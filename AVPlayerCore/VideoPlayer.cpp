#include "stdafx.h"
#include "VideoPlayer.h"
#include "DecoderFactory.h"




CVideoPlayer::CVideoPlayer()
	: m_queueFrame(5)
	, m_queuePacket(MAX_VIDEO_SIZE)
{
	
}

CVideoPlayer::~CVideoPlayer()
{
}

void CVideoPlayer::SetRender(std::shared_ptr<IRender> &render)
{
	m_pRender = render;
}

void CVideoPlayer::SetStream(AVStream * pStream)
{
	m_pStream = pStream;
}

void CVideoPlayer::SetClockMgr(CClockMgr * clockMgr)
{
	m_pClockMgr = clockMgr;
}

bool CVideoPlayer::Open(PLAYER_OPTS &opts)
{
	if (m_bOpen)
		return false;

	m_pDecoder = CDecoderFactory::getSingleModule().CreateDecoder();
	
	m_opts = opts;
	if (!CreateDecoder())
		return false;

	if (m_pDecoder)
		m_pDecoder->Init(m_pCodecCtx);

	if (m_pRender)
		m_pRender->CreateRender(m_opts.hWnd, m_nWndWidth, m_nWndHeight, m_typeCodec == AV_HWDEVICE_TYPE_NONE ? m_pCodecCtx->pix_fmt : AV_PIX_FMT_NV12);
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
	RenderThread();

	m_bPlaying = true;
}

bool CVideoPlayer::IsPlaying()
{
	return m_bPlaying;
}

void CVideoPlayer::Pause(bool IsPause)
{
	m_bPause = IsPause;
}

bool CVideoPlayer::IsPaused()
{
	return m_bPause;
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

	if (m_pRender)
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

void CVideoPlayer::Reset()
{

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

	if (m_opts.bGpuDecode)
	{
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
	}

	if (m_typeCodec == AV_HWDEVICE_TYPE_NONE)
		av_log(NULL, AV_LOG_WARNING, "Failed to create special HW device.");

	if ((ret = avcodec_open2(m_pCodecCtx, pCodec, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to open codec for stream #%u\n", m_pStream);
		return false;
	}

	m_nWndWidth = m_pCodecCtx->width;
	m_nWndHeight = m_pCodecCtx->height;

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
			if (m_bPause)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}
			FramePtr pFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
			if (m_pDecoder && m_pDecoder->DecodeFrame(pFrame, m_queuePacket) >= 0)
			{
				//{
				//	static double temp_pts = 0.0;
				//	static double diff_count = 0.0;
				//	static double diff_average = 0.0;
				//	static int64_t frame_count = 0;
				//	frame_count++;
				//	double pts = (pFrame->best_effort_timestamp - m_pStream->start_time) * av_q2d(m_pStream->time_base);
				//	double diff = pts - temp_pts;
				//	if (diff_average != 0.0 && std::fabs(diff - diff_average* 1000) > 10.0f)
				//		pFrame->pts = (double)diff_average * 1000 / av_q2d(m_pStream->time_base);
				//	//av_log(NULL, AV_LOG_INFO, "frame pts: %d", pFrame->pts);
				//	//av_log(NULL, AV_LOG_INFO, "diff: %f", diff);
				//	temp_pts = pts;
				//	diff_count += diff;
				//	diff_average = diff_count / double(frame_count);
				//	//av_log(NULL, AV_LOG_INFO, "pts:%d, diff:%f, frame pts:%d", pts, diff_average, pFrame->pts/1000);
				//}
				m_queueFrame.Push(std::move(pFrame));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
			if (m_bPause)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			FramePtr pFrame{ nullptr, [](AVFrame* p) { av_frame_free(&p); } };
			if (m_queueFrame.Pop(pFrame))
			{
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
					double audio_pts;//音频pts
					while (!m_bStopRender && !m_bPause && !m_bQuit)
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

				if (m_bPause)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}
			}

			if (m_pRender)
				m_pRender->RenderFrameData(std::move(pFrame));
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
		int frame_rate = m_pStream->avg_frame_rate.den == 0 ? 0 : m_pStream->avg_frame_rate.num / m_pStream->avg_frame_rate.den;//每秒多少帧  
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
