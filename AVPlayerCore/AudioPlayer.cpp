#include "stdafx.h"
#include "AudioPlayer.h"
#include "DecoderFactory.h"
#include <functional>

CAudioPlayer::CAudioPlayer(AVStream* pStream, ISound *pSound)
	:m_pStream(pStream)
	,m_pSound(pSound)
	,m_queueFrame(MAX_AUDIO_SIZE)
	,m_queuePacket(MAX_AUDIO_SIZE)
{
	m_pSound->InitAudio();
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
	bool bRet = m_pSound->OpenAudio(m_pCodecCtx->sample_rate, m_pCodecCtx->channels, m_pCodecCtx->channel_layout);
	if (!bRet)
		return false;
	m_pSound->SetCallback(std::bind(&CAudioPlayer::funcDecodeFrame, this, std::placeholders::_1));
	av_log(NULL, AV_LOG_INFO, "Audio Player Open Audio Device Success");

	m_queueFrame.Init();
	m_queuePacket.Init();

	m_bOpen = true;
	m_opts = opts;
	return true;
}

double CAudioPlayer::GetClock()
{
	return m_clock;
}

void CAudioPlayer::PushPacket(PacketPtr && packet_ptr)
{
	m_queuePacket.Push(std::move(packet_ptr));
}

void CAudioPlayer::ClearFrame()
{
	m_queuePacket.Quit();
	m_queueFrame.Quit();
	avcodec_flush_buffers(m_pCodecCtx);
	m_queuePacket.Init();
	m_queueFrame.Init();
	m_clock = 0.0;
	m_pts = 0.0;
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

	m_pDecoder = CSingleModule<CDecoderFactory>::getSingleModule().CreateDecoder();
	m_pDecoder->Init(m_pCodecCtx);
	return true;
}

int CAudioPlayer::funcDecodeFrame(uint8_t ** buffer)
{
	int nDataSize, nResampledDataSize;
	int64_t dec_channel_layout;
	int wanted_nb_samples;

	FramePtr frame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
	if (m_pDecoder->DecodeFrame(frame.get(), m_queuePacket) < 0)
		return -1;

	if (frame->channels > 0 && frame->channel_layout == 0)
		frame->channel_layout = av_get_default_channel_layout(frame->channels);
	else if (frame->channels == 0 && frame->channel_layout > 0)
		frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

	if (m_pSwrCtx)
		swr_free(&m_pSwrCtx);

	m_pSwrCtx = swr_alloc_set_opts(nullptr, m_pSound->GetWantedChannelLayout(), AV_SAMPLE_FMT_S16, m_pSound->GetWantedSampleRate(),
		frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate, 0, nullptr);

	if (!m_pSwrCtx || swr_init(m_pSwrCtx) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "swr alloc or init error!!!");
		return -1;
	}

	int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_pSwrCtx, frame->sample_rate) + frame->nb_samples,
		m_pSound->GetWantedSampleRate(), AUDIO_S16SYS, AVRounding(1));

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

	*buffer = m_bufferTemp;
	nResampledDataSize = len * m_pSound->GetWantedChannels() * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

	AVRational tb{ 1, frame->sample_rate };
	double pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
	if (!isnan(pts))
		m_clock = pts + (double)frame->nb_samples / (double)frame->sample_rate;
	else
		m_clock = NAN;
	return nResampledDataSize;
}
