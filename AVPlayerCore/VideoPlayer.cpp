#include "stdafx.h"
#include "VideoPlayer.h"
#include "DecoderFactory.h"
#include "RenderFactory.h"
#include "GlRender.h"
#include "D3DRender.h"
#include "libavutil\hwcontext.h"

typedef struct DXVA2DevicePriv {
	HMODULE d3dlib;
	HMODULE dxva2lib;

	HANDLE device_handle;

	IDirect3D9       *d3d9;
	IDirect3DDevice9 *d3d9device;
} DXVA2DevicePriv;

CVideoPlayer::CVideoPlayer()
	: m_queueFrame(5)
	, m_queuePacket(MAX_VIDEO_SIZE)
{
	m_pDecoder = CDecoderFactory::getSingleModule().CreateDecoder();
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
	
	m_opts = opts;
	if (!CreateDecoder())
		return false;

	if (m_pDecoder)
		m_pDecoder->Init(m_pCodecCtx);

	av_log(NULL, AV_LOG_INFO, "Create Decoder");
	m_queueFrame.Init();
	m_queuePacket.Init();

	m_bOpen = true;
	m_bPlaying = false;
	m_bQuit = false;
	m_bStopDecode = false;
	m_bStopRender = false;
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

	m_queuePacket.Quit();
	m_queueFrame.Quit();

	if (m_threadDecode.joinable())
		m_threadDecode.join();

	if (m_threadRender.joinable())
		m_threadRender.join();

	if (m_pCodecCtx)
		avcodec_free_context(&m_pCodecCtx);

	if (m_pHWDeviceCtx)
		av_buffer_unref(&m_pHWDeviceCtx);

	m_bOpen = false;
	m_bPlaying = false;
	m_clock = 0.0;
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

	if (m_opts.decode_type != DECODE_TYPE_NONE)
	{
		bool bGPUDecode = false;
		if (/*NORMAL_TYPE == m_opts.video_type &&*/ DECODE_TYPE_DXVA2 == m_opts.decode_type)
		{
			m_pCodecCtx->thread_count = 1;
			InputStream *ist = new InputStream();
			ist->hwaccel_id = HWACCEL_AUTO;
			ist->hwaccel_device = "dxva2";
			ist->dec = pCodec;
			ist->dec_ctx = m_pCodecCtx;

			m_pCodecCtx->opaque = ist;
			if ((ret = dxva2_init(m_pCodecCtx, m_opts.hWnd)) == 0)
			{
				m_pCodecCtx->get_buffer2 = ist->hwaccel_get_buffer;
				m_pCodecCtx->get_format = GetHwFormat;
				m_pCodecCtx->thread_safe_callbacks = 1;

				m_typeCodec = AV_HWDEVICE_TYPE_DXVA2;
				bGPUDecode = true;
				av_log(NULL, AV_LOG_INFO, "Use Dxva2 GPU Decode");
			}
		}
		else
		{
			av_log(NULL, AV_LOG_INFO, "Use Type:%d GPU Decode", m_opts.decode_type);
			if ((ret = av_hwdevice_ctx_create(&m_pHWDeviceCtx, (AVHWDeviceType)m_opts.decode_type, NULL, NULL, 0)) == 0)
			{
				av_log(NULL, AV_LOG_INFO, "create %d hw device ctx", m_opts.decode_type);
				m_pCodecCtx->hw_device_ctx = av_buffer_ref(m_pHWDeviceCtx);
				m_typeCodec = (AVHWDeviceType)m_opts.decode_type;
				bGPUDecode = true;
			}
		}

		if (!bGPUDecode)
		{
			av_log(NULL, AV_LOG_ERROR, "Create GPU Decoder Failed!!!, err:%d", ret);
			return false;
		}
	}

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
			DWORD dwDecodeStart = ::GetTickCount();
			if (m_pDecoder && m_pDecoder->DecodeFrame(pFrame, m_queuePacket) >= 0)
			{
				DWORD dwDecodeEnd = ::GetTickCount();
				if (pFrame->format == AV_PIX_FMT_DXVA2_VLD /*&& m_opts.video_type == NORMAL_TYPE*/)
				{
					if (m_opts.video_type == NORMAL_TYPE)
					{
						HwRenderFrame(pFrame.get());
					}
					else
					{
						FramePtr pTempFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
						pTempFrame->format = AV_PIX_FMT_YUV420P;
						dxva2_retrieve_data_call2(m_pCodecCtx, pTempFrame.get(), pFrame.get());
						av_frame_copy_props(pTempFrame.get(), pFrame.get());
						m_queueFrame.Push(std::move(pTempFrame));
					}
				}
				else
				{
					if (pFrame->hw_frames_ctx)
					{
						DWORD dwTransferStart = ::GetTickCount();
						FramePtr pTempFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
						int ret = 0;
						if ((ret = av_hwframe_transfer_data(pTempFrame.get(), pFrame.get(), 0)) < 0)
						{
							av_log(NULL, AV_LOG_ERROR, "Error transferring the data to system memory. err:%d", ret);
							continue;
						}
						DWORD dwTransferEnd = ::GetTickCount();
						av_log(NULL, AV_LOG_INFO, "transfer time interval:%d.", dwTransferEnd - dwTransferStart);
						av_frame_copy_props(pTempFrame.get(), pFrame.get());
						m_queueFrame.Push(std::move(pTempFrame));
					}
					else
					{
						av_log(NULL, AV_LOG_INFO, "decode time interval:%d.", dwDecodeEnd - dwDecodeStart);
						m_queueFrame.Push(std::move(pFrame));
					}
						
				}		
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
		CreateRender();

		while (!m_bStopRender && !m_bQuit)
		{
			if (m_bPause)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			FramePtr pFrame{ nullptr, [](AVFrame* p) { av_frame_free(&p); } };
			if (m_queueFrame.Pop(pFrame))
				RenderFrame(pFrame.get());

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (m_pRender)
			m_pRender->DestoryRender();
		av_log(NULL, AV_LOG_INFO, "Video Render Thread End!!!");
	});
}

bool CVideoPlayer::CreateRender()
{
	if (m_pRender)
	{
		bool bRet = m_pRender->CreateRender(m_opts.hWnd, m_pCodecCtx->width, m_pCodecCtx->height, m_typeCodec == AV_HWDEVICE_TYPE_NONE ? m_pCodecCtx->pix_fmt : AV_PIX_FMT_NV12);
		if (!bRet && m_opts.funcEvent)
		{
			PlayerErrorSt err;
			err.nCode = GlCreateError;
			err.strErrMsg = "create gl render failed!!!";
			m_opts.funcEvent(m_opts.user_data, PlayerError, &err);
		}
		return bRet;
	}
	return false;
}

void CVideoPlayer::RenderFrame(AVFrame *pFrame)
{
	double video_pts = 0; //当前视频的pts
	if (pFrame->pts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE)
		video_pts = *(int64_t *)pFrame->opaque;
	else if (pFrame->pts != AV_NOPTS_VALUE)
		video_pts = pFrame->pts;
	else
		video_pts = 0;

	video_pts *= av_q2d(m_pStream->time_base);
	video_pts = SyncVideo(pFrame, video_pts);

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
		float delay = SmoothVideo(pFrame, m_queuePacket.Count());
		::Sleep(delay);
	}

	if (m_bPause)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return;
	}

	if (m_pRender)
		m_pRender->RenderFrameData(pFrame);
}

void CVideoPlayer::HwRenderFrame(AVFrame * pFrame)
{
	double video_pts = 0; //当前视频的pts
	if (pFrame->pts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE)
		video_pts = *(int64_t *)pFrame->opaque;
	else if (pFrame->pts != AV_NOPTS_VALUE)
		video_pts = pFrame->pts;
	else
		video_pts = 0;

	video_pts *= av_q2d(m_pStream->time_base);
	video_pts = SyncVideo(pFrame, video_pts);

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
		float delay = SmoothVideo(pFrame, m_queuePacket.Count());
		::Sleep(delay);
	}

	if (m_bPause)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return;
	}

	D3DSURFACE_DESC    surfaceDesc;
	D3DLOCKED_RECT     LockedRect;
	int lock_flags = 0;

	dxva2_retrieve_data_call(m_pCodecCtx, pFrame);

	//LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9)pFrame->data[3];
	//if (!surface)
	//{
	//	av_log(NULL, AV_LOG_ERROR, "frame data[3] to LPDIRECT3DSURFACE9 failed!!!");
	//	return;
	//}

	//AVHWFramesContext *ctx = (AVHWFramesContext*)pFrame->hw_frames_ctx->data;
	//DXVA2DevicePriv *priv = (DXVA2DevicePriv*)ctx->device_ctx->user_opaque;
	//if (m_pDirect3DSurfaceRender)
	//{
	//	m_pDirect3DSurfaceRender->Release();
	//	m_pDirect3DSurfaceRender = NULL;
	//}
	////D3DRS_MUTISAMPLEANTIALIAS
	//DWORD dwValue = 0;
	//HRESULT hr = priv->d3d9device->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &dwValue);
	//if (!m_bSetMulitSamples)
	//{
	//	D3DPRESENT_PARAMETERS d3dpp;
	//	ZeroMemory(&d3dpp, sizeof(d3dpp));
	//	d3dpp.BackBufferCount = 0;
	//	d3dpp.BackBufferHeight = 0;
	//	d3dpp.BackBufferWidth = 0;
	//	hr = priv->d3d9device->Reset(&d3dpp);
	//	if (hr != D3D_OK)
	//		av_log(NULL, AV_LOG_ERROR, "device reset: %d", GetLastError());
	//	hr = priv->d3d9device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	//	m_bSetMulitSamples = true;
	//}
	//priv->d3d9device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//priv->d3d9device->BeginScene();
	//priv->d3d9device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pDirect3DSurfaceRender);
	//priv->d3d9device->StretchRect(surface, NULL, m_pDirect3DSurfaceRender, NULL, D3DTEXF_LINEAR);
	//priv->d3d9device->EndScene();
	//priv->d3d9device->Present(NULL, NULL, m_opts.hWnd, NULL);
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

AVPixelFormat CVideoPlayer::GetHwFormat(AVCodecContext * s, const AVPixelFormat * pix_fmts)
{
	InputStream* ist = (InputStream*)s->opaque;
	ist->hwaccel_id = HWACCEL_DXVA2;
	ist->hwaccel_pix_fmt = AV_PIX_FMT_DXVA2_VLD;
	return ist->hwaccel_pix_fmt;
}

