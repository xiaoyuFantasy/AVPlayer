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
	avcodec_flush_buffers(m_pCodecCtx);
	m_queuePacket.Init();
	m_queueFrame.Init();
	m_nBufferIndex = 0;
	m_nBufferSize = 0;
	m_clock = 0.0;
	m_pts = 0.0;
	m_bStopDecode = false;
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
			audio_size = p->GetFrame();
			if (audio_size < 0)
			{
				p->m_buffer = nullptr;
				p->m_nBufferSize = AUDIO_MIN_BUFFER_SIZE / p->m_hwParams.frame_size * p->m_hwParams.frame_size;
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
		
		len -= len1;
		stream += len1;
		p->m_nBufferIndex += len1;
	}

	p->m_pts = p->m_clock - (double)(2 * p->m_hwSize + p->m_nBufferSize - p->m_nBufferIndex) / p->m_hwParams.bytes_per_sec;
}

int CAudioPlayer::GetFrame()
{
	int nDataSize, nResampledDataSize;
	int64_t dec_channel_layout;
	int wanted_nb_samples;

	if (m_bQuit || m_bPaused)
		return -1;

	FramePtr frame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
	
	if (DecodeFrame(frame.get()) < 0)
		return -1;
	
	if (frame->channels > 0 && frame->channel_layout == 0)
		frame->channel_layout = av_get_default_channel_layout(frame->channels);
	else if (frame->channels == 0 && frame->channel_layout > 0)
		frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

	if (m_pSwrCtx)
		swr_free(&m_pSwrCtx);

	m_pSwrCtx = swr_alloc_set_opts(nullptr, m_hwParams.channel_layout, AV_SAMPLE_FMT_S16, m_hwParams.freq,
		frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate, 0, nullptr);

	if (!m_pSwrCtx || swr_init(m_pSwrCtx) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "swr alloc or init error!!!");
		return -1;
	}

	int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_pSwrCtx, frame->sample_rate) + frame->nb_samples,
		m_hwParams.freq, m_hwParams.fmt, AVRounding(1));

	ZeroMemory(m_bufferTemp, sizeof(m_bufferTemp));
	uint8_t *out[] = { m_bufferTemp };
	int out_count = sizeof(m_bufferTemp) / 2 / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
	int len = swr_convert(m_pSwrCtx, (uint8_t **)&out, dst_nb_samples,
		(const uint8_t**)frame->data, frame->nb_samples);

	if (len < 0) 
	{
		av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
		return -1;
	}

	m_buffer = m_bufferTemp;
	nResampledDataSize = len * m_hwParams.channels * av_get_bytes_per_sample(m_hwParams.fmt);
	
	AVRational tb{ 1, frame->sample_rate };
	double pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
	if (!isnan(pts))
		m_clock = pts + (double)frame->nb_samples / (double)frame->sample_rate;
	else
		m_clock = NAN;
	return nResampledDataSize;
}

int CAudioPlayer::DecodeFrame(AVFrame * pFrame)
{
	int ret = AVERROR(EAGAIN);
	for (;;)
	{
		do
		{
			ret = avcodec_receive_frame(m_pCodecCtx, pFrame);
			if (ret >= 0)
			{
				AVRational tb{ 1, pFrame->sample_rate };
				if (pFrame->pts != AV_NOPTS_VALUE)
					pFrame->pts = av_rescale_q(pFrame->pts, m_pCodecCtx->pkt_timebase, tb);
			}

			if (ret == AVERROR_EOF)
			{
				avcodec_flush_buffers(m_pCodecCtx);
				return -1;
			}

			if (ret >= 0)
				return 1;
		} while (ret != AVERROR(EAGAIN));

		PacketPtr packet{ nullptr, [](AVPacket* p) { av_packet_free(&p); } };
		if (m_queuePacket.Pop(packet))
		{
			if (avcodec_send_packet(m_pCodecCtx, packet.get()) == AVERROR(EAGAIN))
			{
				char szErr[512] = { 0 };
				av_strerror(ret, szErr, sizeof(szErr));
				av_log(NULL, AV_LOG_ERROR, "video codec send packet error. err:%d, %s", ret, szErr);
			}
		}
	}
	return 0;
}
