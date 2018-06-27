#include "stdafx.h"
#include "AudioPlayer.h"


CAudioPlayer::CAudioPlayer(AVStream* pStream)
	:m_pStream(pStream)
	,m_queueFrame(MAX_AUDIO_SIZE)
	,m_queuePacket(MAX_AUDIO_SIZE)
{
}


CAudioPlayer::~CAudioPlayer()
{
}

bool CAudioPlayer::Open(PLAYER_OPTS &opts)
{
	if (m_bOpen)
		return false;

	if (!CreateDecoder())
		return false;
	av_log(NULL, AV_LOG_INFO, "Audio Player Create Decoder Success");
	
	if (!OpenAudioDevice())
		return false;
	av_log(NULL, AV_LOG_INFO, "Audio Player Open Audio Device Success");

	m_queueFrame.Init();
	m_queuePacket.Init();

	m_bOpen = true;
	m_bQuit = false;
	m_bPlaying = false;
	m_opts = opts;
	return true;
}

void CAudioPlayer::Play()
{
	if (!m_bOpen || m_bPlaying)
		return;

	m_bPlaying = true;
	SDL_PauseAudioDevice(m_audioDevID, 0);
	DecodeThread();
}

bool CAudioPlayer::IsPlaying()
{
	return m_bPlaying;
}

void CAudioPlayer::Pause(bool IsPause)
{
	m_bPaused = IsPause;
}

bool CAudioPlayer::IsPaused()
{
	return false;
}

void CAudioPlayer::Stop()
{
	if (!m_bOpen || !m_bPlaying)
		return;

	m_bQuit = true;
	SDL_CloseAudioDevice(m_audioDevID);
	if (m_threadDecode.joinable())
		m_threadDecode.join();

	m_bPlaying = false;
	m_bOpen = false;
}

double CAudioPlayer::GetClock()
{
	return m_clock;
}

void CAudioPlayer::SetVolume(int nVolume)
{
	if (nVolume < 0)
		m_nVolume = 0;
	if (nVolume > SDL_MIX_MAXVOLUME)
		m_nVolume = SDL_MIX_MAXVOLUME;
	else
		m_nVolume = nVolume;
}

int CAudioPlayer::GetVolume() const
{
	return m_nVolume;
}

void CAudioPlayer::SetMuted(bool bMuted)
{
	m_bMuted = bMuted;
}

bool CAudioPlayer::IsMuted() const
{
	return m_bMuted;
}

void CAudioPlayer::PushPacket(PacketPtr && packet_ptr)
{
	m_queuePacket.Push(std::move(packet_ptr));
}

void CAudioPlayer::ClearFrame()
{
	m_bStopDecode = true;
	m_queuePacket.Quit();
	m_queueFrame.Quit();
	if (m_threadDecode.joinable())
		m_threadDecode.join();
	avcodec_flush_buffers(m_pCodecCtx);
	m_queuePacket.Init();
	m_queueFrame.Init();
	m_nBufferIndex = 0;
	m_nBufferSize = 0;
	m_clock = 0.0;
	m_pts = 0.0;
	m_bStopDecode = false;
	DecodeThread();
}

bool CAudioPlayer::CreateDecoder()
{
	int ret = 0;
	AVCodec *pCodec = avcodec_find_decoder(m_pStream->codecpar->codec_id);
	if (!pCodec)
	{
		av_log(NULL, AV_LOG_ERROR, "Can Not Found AVCodec.");
		return false;
	}

	if (m_pCodecCtx)
		avcodec_free_context(&m_pCodecCtx);

	if (!(m_pCodecCtx = avcodec_alloc_context3(pCodec)))
	{
		av_log(NULL, AV_LOG_ERROR, "Alloc AVCodecContext Failed. err:%d", AVERROR(ENOMEM));
		return false;
	}
	ret = avcodec_parameters_to_context(m_pCodecCtx, m_pStream->codecpar);
	if (ret < 0)
	{
		char szErr[AV_ERROR_MAX_STRING_SIZE];
		av_strerror(ret, szErr, AV_ERROR_MAX_STRING_SIZE);
		av_log(NULL, AV_LOG_ERROR, "AVCodecParameters To AVCodecContext Failed. err:%d, %s", ret, szErr);
		return false;
	}

	if ((ret = avcodec_open2(m_pCodecCtx, pCodec, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to open codec for stream #%u\n", m_pStream);
		return false;
	}

	return true;
}

bool CAudioPlayer::OpenAudioDevice()
{
	if (m_bPaused)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return -1;
	}

	int wanted_sample_rate = m_pCodecCtx->sample_rate;
	int wanted_nb_channels = m_pCodecCtx->channels;
	int64_t wanted_channel_layout = m_pCodecCtx->channel_layout;

	SDL_AudioSpec wanted_spec, spec;
	const char *env;
	const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
	const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env)
	{
		wanted_nb_channels = atoi(env);
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
	}

	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout))
	{
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}

	wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
	wanted_spec.channels = wanted_nb_channels;
	wanted_spec.freq = wanted_sample_rate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
		return false;
	}

	while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
		next_sample_rate_idx--;

	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = FFMAX(AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = this;

	while (!(m_audioDevID = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE)))
	{
		av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n", wanted_spec.channels, wanted_spec.freq, SDL_GetError());
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels)
		{
			wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			wanted_spec.channels = wanted_nb_channels;
			if (!wanted_spec.freq)
			{
				av_log(NULL, AV_LOG_ERROR, "No more combinations to try, audio open failed\n");
				return false;
			}
		}
		wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
	}

	if (spec.format != AUDIO_S16SYS)
	{
		av_log(NULL, AV_LOG_ERROR, "SDL advised audio format %d is not supported!\n", spec.format);
		return false;
	}

	if (spec.channels != wanted_spec.channels)
	{
		wanted_channel_layout = av_get_default_channel_layout(spec.channels);
		if (!wanted_channel_layout)
		{
			av_log(NULL, AV_LOG_ERROR, "SDL advised channel count %d is not supported!\n", spec.channels);
			return false;
		}
	}

	m_hwParams.fmt = AV_SAMPLE_FMT_S16;
	m_hwParams.freq = spec.freq;
	m_hwParams.channel_layout = wanted_channel_layout;
	m_hwParams.channels = spec.channels;
	m_hwParams.frame_size = av_samples_get_buffer_size(NULL, m_hwParams.channels, 1, m_hwParams.fmt, 1);
	m_hwParams.bytes_per_sec = av_samples_get_buffer_size(NULL, m_hwParams.channels, m_hwParams.freq, m_hwParams.fmt, 1);
	if (m_hwParams.bytes_per_sec <= 0 || m_hwParams.frame_size <= 0) {
		av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return false;
	}
	m_hwSize = spec.size;
	return true;
}

void CAudioPlayer::DecodeThread()
{
	m_threadDecode = std::thread([&]() {
		av_log(NULL, AV_LOG_INFO, "Audio Decode Thread");
		while (!m_bStopDecode && !m_bQuit)
		{
			if (m_bPaused)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			PacketPtr packet{ nullptr, [](AVPacket* p) { av_packet_free(&p); } };
			if (!m_queuePacket.Pop(packet))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
				continue;
			}

			int ret = avcodec_send_packet(m_pCodecCtx, packet.get());
			if (ret != 0 && ret != AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				char szErr[512] = { 0 };
				av_strerror(ret, szErr, sizeof(szErr));
				av_log(NULL, AV_LOG_ERROR, "video codec send packet error. err:%d, %s", ret, szErr);
				continue;
			}

			while ((ret == 0 || ret == AVERROR(EAGAIN)) && !m_bStopDecode && !m_bQuit)
			{
				FramePtr pFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
				ret = avcodec_receive_frame(m_pCodecCtx, pFrame.get());
				if (ret == 0)
					m_queueFrame.Push(std::move(pFrame));
				else
					break;
			}
		}
	});
}

void CAudioPlayer::audio_callback(void * userData, Uint8 * stream, int len)
{
	CAudioPlayer* p = (CAudioPlayer*)userData;
	int audio_size, len1;

	p->m_nCallbackTime = av_gettime_relative();
	
	while (len > 0 && !p->m_bQuit)
	{
		if (p->m_bPaused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		/*
		m_nAudioDataIndex 和 m_nAudioBufferSize 标示我们自己用来放置解码出来的数据的缓冲区,
		这些数据待copy到SDL缓冲区, 当m_nAudioDataIndex >= m_nAudioBufferSize的时候意味着我
		们的缓冲为空, 没有数据可供copy, 这时候需要调用audio_decode_frame来解码出更多的桢数据
		*/
		if (p->m_nBufferIndex >= p->m_nBufferSize)
		{
			audio_size = p->DecodeFrame();
			if (audio_size < 0)
			{
				p->m_buffer = nullptr;
				p->m_nBufferIndex = AUDIO_MIN_BUFFER_SIZE / p->m_hwParams.frame_size * p->m_hwParams.frame_size;
			}
			else
			{
				p->m_nBufferSize = audio_size;
			}

			p->m_nBufferIndex = 0;
		}
		
		len1 = p->m_nBufferSize - p->m_nBufferIndex;
		if (len1 > len)
			len1 = len;

		if (p->m_bMuted && p->m_buffer && p->m_nVolume == SDL_MIX_MAXVOLUME)
			memcpy(stream, (uint8_t *)p->m_buffer + p->m_nBufferIndex, len1);
		else
		{
			memset(stream, 0, len1);
			if (!p->m_bMuted && p->m_buffer)
				SDL_MixAudioFormat(stream, (uint8_t *)p->m_buffer + p->m_nBufferIndex, AUDIO_S16SYS, len1, p->m_nVolume);
		}

		//if (p->m_buffer)
		//	//memcpy(stream, (uint8_t *)p->m_buffer + p->m_nBufferIndex, len1);
		//	SDL_MixAudio(stream, (uint8_t *)p->m_buffer + p->m_nBufferIndex, len1, p->m_nVolume);
		//else
		//	memset(stream, 0, len1);
		
		len -= len1;
		stream += len1;
		p->m_nBufferIndex += len1;
	}

	p->m_pts = p->m_clock - (double)(2 * p->m_hwSize + p->m_nBufferSize - p->m_nBufferIndex) / p->m_hwParams.bytes_per_sec;
}

int CAudioPlayer::DecodeFrame()
{
	int nDataSize, nResampledDataSize;
	int64_t dec_channel_layout;
	int wanted_nb_samples;

	while (m_queueFrame.Count() == 0 && !m_bQuit)
	{
		if ((av_gettime_relative() - m_nCallbackTime) > 1000000LL * m_nBufferSize / m_hwParams.bytes_per_sec / 2)
			return -1;
		av_usleep(1000);
	}

	if (m_bQuit)
		return -1;

	FramePtr frame{ av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); } };

	if (!m_queueFrame.Pop(frame))
		return -1;

	nDataSize = av_samples_get_buffer_size(nullptr, frame->channels, frame->nb_samples, (AVSampleFormat)frame->format, 1);
	dec_channel_layout = (frame->channel_layout && frame->channels == av_get_channel_layout_nb_channels(frame->channel_layout)) ? frame->channel_layout : av_get_default_channel_layout(frame->channels);
	wanted_nb_samples = frame->nb_samples;

	if (frame->format != m_hwParams.fmt ||
		dec_channel_layout != m_hwParams.channel_layout ||
		frame->sample_rate != m_hwParams.freq ||
		wanted_nb_samples != frame->nb_samples &&
		!m_pSwrCtx)
	{
		//设置转换参数
		if (m_pSwrCtx)
		{
			swr_free(&m_pSwrCtx);
			m_pSwrCtx = nullptr;
		}

		m_pSwrCtx = swr_alloc_set_opts(nullptr,
			m_hwParams.channel_layout,
			m_hwParams.fmt,
			m_hwParams.freq,
			dec_channel_layout,
			(AVSampleFormat)frame->format,
			frame->sample_rate,
			0,
			NULL);

		if (!m_pSwrCtx || swr_init(m_pSwrCtx) < 0)
		{
			av_log(NULL, AV_LOG_ERROR,
				"Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
				frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)frame->format), frame->channels,
				m_hwParams.freq, av_get_sample_fmt_name(m_hwParams.fmt), m_hwParams.channels);
			return -1;
		}

		const uint8_t **in = (const uint8_t **)frame->extended_data;
		uint8_t *out[] = { m_bufferTemp };
		int out_count = sizeof(m_bufferTemp) / 2 / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);;//(int64_t)wanted_nb_samples * m_hwParams.freq / frame->sample_rate + 256;
		int out_size = av_samples_get_buffer_size(NULL, m_hwParams.channels, out_count, m_hwParams.fmt, 0);
		if (out_size < 0) {
			av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
			return -1;
		}

		if (wanted_nb_samples != frame->nb_samples) {
			if (swr_set_compensation(m_pSwrCtx, (wanted_nb_samples - frame->nb_samples) * m_hwParams.freq / frame->sample_rate,
				wanted_nb_samples * m_hwParams.freq / frame->sample_rate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
				return -1;
			}
		}

		int len = swr_convert(m_pSwrCtx, out, out_count, in, frame->nb_samples);
		swr_free(&m_pSwrCtx);
		if (len < 0) {
			av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
			return -1;
		}

		m_buffer = m_bufferTemp;
		nResampledDataSize = len * m_hwParams.channels * av_get_bytes_per_sample(m_hwParams.fmt);
	}
	else
	{
		m_buffer = frame->data[0];
		nResampledDataSize = nDataSize;
	}

	AVRational tb{ 1, frame->sample_rate };
	double pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
	if (!isnan(pts))
		m_clock = pts + (double)frame->nb_samples / (double)frame->sample_rate;
	else
		m_clock = NAN;
	return nResampledDataSize;
}
