#include "stdafx.h"
#include "GlRender.h"

CGlRender::CGlRender()
{
}

CGlRender::~CGlRender()
{
}

bool CGlRender::InitRender()
{
	return true;
}

bool CGlRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	if (!::IsWindow(hWnd))
		return false;

	RECT rc;
	GetWindowRect(hWnd, &rc);
	std::string strWndName = CreateGuidToString("MPlayerWnd_");
	PANO_INFO info{ strWndName.c_str(), (char*)strWndName.c_str(), 1, nWidth, nHeight, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, PLAY_MODE::PANO2D, FRAME_FORMAT::VIDEO };
	m_hNativeRender = NativeOnCreate(hWnd, nullptr, nullptr, &info);
	if (nullptr == m_hNativeRender)
	{
		av_log(NULL, AV_LOG_ERROR, "NativeOnCreate Failed!!!");
		return false;
	}

	m_hWnd = hWnd;
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
	m_nPixelFormat = pixelFormat;

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	m_pFrameOut = av_frame_alloc();
	m_pFrameOut->format = AV_PIX_FMT_YUV420P;
	m_pFrameOut->width = m_nVideoWidth;
	m_pFrameOut->height = m_nVideoHeight;

	m_pSwsCtx = sws_getContext(
		m_nVideoWidth,
		m_nVideoHeight,
		(AVPixelFormat)pixelFormat,
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
		DestoryRender();
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

	return true;
}

void CGlRender::DestoryRender()
{
	m_bClose = true;

	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

}

bool CGlRender::SetRenderMode(RENDER_MODE)
{
	return false;
}

void CGlRender::SetRenderSize(int width, int height)
{
	if (height == 0) 
		height = 1;

	m_nWndWidth = width;
	m_nWndHeight = height;

	if (m_hNativeRender != nullptr)
		NativeUpdateViewport(m_hNativeRender, 0, 0, m_nWndWidth, m_nWndHeight);
}

void CGlRender::RenderFrameData(FramePtr pFrame)
{
	if (m_bClose)
		return;

	int nHeight = sws_scale(
		m_pSwsCtx,
		(uint8_t const * const *)pFrame->data,
		pFrame->linesize,
		0,
		pFrame->height,
		m_pFrameOut->data,
		m_pFrameOut->linesize);

	if (m_hNativeRender != nullptr)
		NativeSetFrameData(m_hNativeRender, STREAM_FORMAT::FRAME_FORMAT_YUV, m_pFrameOut->data);
}

void CGlRender::SetScale(float factor)
{
}

void CGlRender::SetRotate(float x, float y)
{
}

void CGlRender::SetScroll(float latitude, float longitude)
{
}

void CGlRender::SetReverse(bool filp)
{
}

void CGlRender::InitGL()
{
}