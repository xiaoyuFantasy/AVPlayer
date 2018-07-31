#include "stdafx.h"
#include "VideoPlayer.h"
#include "DecoderFactory.h"
#include <Objbase.h>
#pragma comment(lib, "Ole32.lib")

std::string CreateGuidToString(char *str)
{
	GUID guid;
	CoCreateGuid(&guid);
	char buf[MAX_PATH] = { 0 };
	_snprintf_s(
		buf,
		sizeof(buf),
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);

	std::string strTemp;
	if (str)
		strTemp.append(str);

	strTemp.append(buf);
	return strTemp;
}

CVideoPlayer::CVideoPlayer(AVStream* pStream, IRender* render)
	: m_queueFrame(50)
	, m_pStream(pStream)
	, m_pRender(render)
	, m_queuePacket(MAX_VIDEO_SIZE)
{
	m_pDecoder = CDecoderFactory::getSingleModule().CreateDecoder();
}

CVideoPlayer::~CVideoPlayer()
{
}

void CVideoPlayer::SetClockMgr(CClockMgr * clockMgr)
{
	m_pClockMgr = clockMgr;
}

bool CVideoPlayer::Open(PLAYER_OPTS &opts)
{
	if (m_bOpen)
		return false;

	m_opts = opts;
	if (!CreateDecoder())
		return false;

	m_pRender->CreateRender(opts.hWnd, m_nWndWidth, m_nWndHeight);
	av_log(NULL, AV_LOG_INFO, "Create Decoder");
	m_queueFrame.Init();
	m_queuePacket.Init();

	m_bOpen = true;
	m_bPlaying = false;
	m_bQuit = false;
	return true;
}

void CVideoPlayer::Play()
{
	if (!m_bOpen || m_bPlaying)
		return;

	if (m_threadDecode.joinable())
		m_threadDecode.join();

	DecodeThread();
	//RenderThread();

	m_bPlaying = true;
}

bool CVideoPlayer::IsPlaying()
{
	return m_bPlaying;
}

void CVideoPlayer::Pause(bool IsPause)
{
	m_bPaused = IsPause;
}

bool CVideoPlayer::IsPaused()
{
	return m_bPaused;
}

void CVideoPlayer::Stop()
{
	if (!m_bOpen || !m_bPlaying)
		return;

	m_bQuit = true;
	if (m_threadDecode.joinable())
		m_threadDecode.join();

	if (m_threadRender.joinable())
		m_threadRender.join();

	if (m_pCodecCtx)
		avcodec_free_context(&m_pCodecCtx);

	if (m_pHWDeviceCtx)
		av_buffer_unref(&m_pHWDeviceCtx);

	m_pRender->DestoryRender();
	m_bPlaying = false;
}

double CVideoPlayer::GetClock()
{
	return m_clock;
}

void CVideoPlayer::ClearFrame()
{
	m_bStopDecode = true;
	m_bStopRender = true;
	m_queuePacket.Quit();
	m_queueFrame.Quit();
	if (m_threadDecode.joinable())
		m_threadDecode.join();
	if (m_threadRender.joinable())
		m_threadRender.join();
	avcodec_flush_buffers(m_pCodecCtx);
	m_queuePacket.Init();
	m_queueFrame.Init();
	m_clock = 0.0;
	m_bStopDecode = false;
	m_bStopRender = false;
	DecodeThread();
	RenderThread();
}

void CVideoPlayer::PushPacket(PacketPtr && packet_ptr)
{
	m_queuePacket.Push(std::move(packet_ptr));
}

bool CVideoPlayer::CreateDecoder()
{
	int ret = 0;
	AVCodec* pCodec = avcodec_find_decoder(m_pStream->codecpar->codec_id);
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

	if (m_pHWDeviceCtx)
		av_buffer_unref(&m_pHWDeviceCtx);

	AVHWDeviceType nHWTypes[3] = { AV_HWDEVICE_TYPE_DXVA2 , AV_HWDEVICE_TYPE_D3D11VA, AV_HWDEVICE_TYPE_CUDA };
	for (size_t i = 0; i < sizeof(nHWTypes); i++)
	{
		if ((ret = av_hwdevice_ctx_create(&m_pHWDeviceCtx, nHWTypes[i], NULL, NULL, 0)) == 0)
		{
			m_pCodecCtx->hw_device_ctx = av_buffer_ref(m_pHWDeviceCtx);
			m_typeCodec = nHWTypes[i];
			break;
		}
	}

	if (m_typeCodec == AV_HWDEVICE_TYPE_NONE)
		av_log(NULL, AV_LOG_WARNING, "Failed to create special HW device.");

	if ((ret = avcodec_open2(m_pCodecCtx, pCodec, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to open codec for stream #%u\n", m_pStream);
		return false;
	}

	m_pDecoder->Init(m_pCodecCtx);

	m_nWndWidth = m_pCodecCtx->width;
	m_nWndHeight = m_pCodecCtx->height;

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);
	m_pFrameOut = av_frame_alloc();
	m_pFrameOut->format = AV_PIX_FMT_YUV420P;
	m_pFrameOut->width = m_pCodecCtx->width;
	m_pFrameOut->height = m_pCodecCtx->height;
	m_pSwsCtx = sws_getContext(
		m_pCodecCtx->width,
		m_pCodecCtx->height,
		AV_PIX_FMT_NV12,
		m_pFrameOut->width,
		m_pFrameOut->height,
		(AVPixelFormat)m_pFrameOut->format,
		SWS_BILINEAR,
		nullptr,
		nullptr,
		nullptr);

	if (!m_pSwsCtx)
	{
		av_log(NULL, AV_LOG_ERROR, "sws_getContext failed.");
		return false;
	}

	int nSize = av_image_get_buffer_size(
		(AVPixelFormat)m_pFrameOut->format,
		m_pFrameOut->width,
		m_pFrameOut->height,
		1);

	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);
	m_pVideoBuffer = (uint8_t*)av_malloc(nSize * sizeof(uint8_t));

	//设置帧数据对应的内存
	av_image_fill_arrays(
		m_pFrameOut->data,
		m_pFrameOut->linesize,
		m_pVideoBuffer,
		(AVPixelFormat)m_pFrameOut->format,
		m_pFrameOut->width,
		m_pFrameOut->height,
		1);

	if (m_opts.videoCb)
		m_opts.videoCb(m_opts.user_data, m_nWndWidth, m_nWndHeight);

	return true;
}

void CVideoPlayer::DecodeThread()
{
	m_threadDecode = std::thread([&]() {
		av_log(NULL, AV_LOG_INFO, "Video Decode Thread");
		if (!m_pCodecCtx)
			return;

		while (!m_bStopDecode && !m_bQuit)
		{
			if (m_bPaused)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}
			FramePtr pFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
			if (m_pDecoder->DecodeFrame(pFrame.get(), m_queuePacket) >= 0)
			{
				av_log(NULL, AV_LOG_INFO, "DecodeFrame");
				m_queueFrame.Push(std::move(pFrame));
			}
			
			/*PacketPtr packet{ nullptr, [](AVPacket* p) { av_packet_free(&p); } };
			if (!m_queuePacket.Pop(packet))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
				continue;
			}

			int ret = avcodec_send_packet(m_pCodecCtx, packet.get());
			if (ret != 0 && ret != AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				av_log(NULL, AV_LOG_ERROR, "video codec send packet error. err:%d", ret);
				continue;
			}

			while ((ret == 0 || ret == AVERROR(EAGAIN)) && !m_bStopDecode && !m_bQuit)
			{
				FramePtr pFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
				FramePtr pSWFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
				ret = avcodec_receive_frame(m_pCodecCtx, pFrame.get());
				if (ret != 0)
					break;

				if (AV_HWDEVICE_TYPE_NONE != m_typeCodec)
				{
					if ((ret = av_hwframe_transfer_data(pSWFrame.get(), pFrame.get(), 0)) < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "Error transferring the data to system memory. err:%d", ret);
						continue;
					}

					pSWFrame->pts = pFrame->pts;
					pSWFrame->pkt_dts = pFrame->pkt_dts;
					pSWFrame->pkt_pos = pFrame->pkt_pos;
					pSWFrame->pkt_duration = pFrame->pkt_duration;
					pSWFrame->pkt_size = pFrame->pkt_size;
					pSWFrame->best_effort_timestamp = pFrame->best_effort_timestamp;
					pSWFrame->repeat_pict = pFrame->repeat_pict;
					m_queueFrame.Push(std::move(pSWFrame));
				}
				else
				{
					m_queueFrame.Push(std::move(pFrame));
				}
			}*/
		}

		av_log(NULL, AV_LOG_INFO, "Video Decode Thread End!!!");
	});
}

void CVideoPlayer::RenderThread()
{
	m_threadRender = std::thread([&]() {
		av_log(NULL, AV_LOG_INFO, "Video Render Thread");
		while (!m_bStopRender && !m_bQuit)
		{
			if (m_bPaused)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			FramePtr pFrame{ nullptr, [](AVFrame* p) { av_frame_free(&p); } };
			if (m_queueFrame.Pop(pFrame))
			{
				//m_queueFrame.Pop(pFrame);
				double video_pts = 0; //当前视频的pts
				if (pFrame->pts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE)
					video_pts = *(int64_t *)pFrame->opaque;
				else if (pFrame->pts != AV_NOPTS_VALUE)
					video_pts = pFrame->pts;
				else
					video_pts = 0;

				video_pts *= av_q2d(m_pStream->time_base);
				video_pts = SyncVideo(pFrame.get(), video_pts);
				if (m_pClockMgr->GetAudioClock() > 0)
				{
					double audio_pts; //音频pts
					while (!m_bStopRender && !m_bPaused && !m_bQuit)
					{
						audio_pts = m_pClockMgr->GetAudioClock();
						//av_log(NULL, AV_LOG_INFO, "video pts:%f, audio pts:%f", video_pts, audio_pts);
						if (video_pts <= audio_pts)
							break;

						int delayTime = (video_pts - audio_pts) * 1000;
						delayTime = delayTime > 5 ? 5 : delayTime;
						std::this_thread::sleep_for(std::chrono::milliseconds(delayTime));
					}
				}
				else
				{
					float delay = SmoothVideo(pFrame.get(), m_queueFrame.Size());
					::Sleep(delay);
				}

				if (m_bPaused)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}

				int nHeight = sws_scale(m_pSwsCtx,
					(uint8_t const * const *)pFrame->data,
					pFrame->linesize, 0, pFrame->height, m_pFrameOut->data,
					m_pFrameOut->linesize);

				if (m_opts.progreeeCb)
					m_opts.progreeeCb(m_opts.user_data, video_pts);
			}

			m_pRender->RenderFrameData(m_pFrameOut);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		av_log(NULL, AV_LOG_INFO, "Video Render Thread End!!!");
	});
}

double CVideoPlayer::SyncVideo(AVFrame * frame, double pts)
{
	double frame_delay;

	/* compute the exact PTS for the picture if it is omitted in the stream
	* pts1 is the dts of the pkt / pts of the frame */
	if (pts != 0) {
		/* update video clock with pts, if present */
		m_clock = pts;
	}
	else {
		pts = m_clock;
	}
	/* update video clock for next frame */
	frame_delay = av_q2d(m_pStream->time_base);
	/* for MPEG2, the frame can be repeated, so we update the
	clock accordingly */
	frame_delay += frame->repeat_pict * (frame_delay * 0.5);
	m_clock += frame_delay;
	return pts;
}

float CVideoPlayer::SmoothVideo(AVFrame * frame, int size)
{
	int64_t duration = frame->pkt_duration;
	if (duration <= 0 || duration >= 100)
	{
		int frame_rate = m_pStream->avg_frame_rate.num / m_pStream->avg_frame_rate.den;//每秒多少帧  
		if (frame_rate > 0)
			duration = 1000 / frame_rate;
		else
			duration = 40;
	}

	int frames_total = 0;
	int capacity = 30;//缓冲大小
	float fShowSpeed = 1;

	frames_total = size;
	if (frames_total <= 5) {
		float speedx = 0;
		speedx = 1.0 / (capacity / 2.0 - frames_total);
		fShowSpeed = 1.0 - speedx * 5;
	}

	if (frames_total >= capacity) {//如果显示帧缓存太多，则倍数播放
		float speedx = 0;
		speedx = (frames_total*1.0) / (capacity*1.0);
		fShowSpeed = 1.0 + speedx * 2 / 10.0;
	}

	float delay = duration / fShowSpeed;
	return delay;
}
