#include "stdafx.h"
#include "SDLRender.h"


CSDLRender::CSDLRender()
{
}


CSDLRender::~CSDLRender()
{
}

bool CSDLRender::InitRender()
{
	SDL_Init(SDL_INIT_VIDEO);
	return true;
}

bool CSDLRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	if (!::IsWindow(hWnd))
		return false;

	if (m_pWindow)
		return false;

	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	RECT rc;
	::GetWindowRect(hWnd, &rc);
	m_nWndWidth = rc.right - rc.left;
	m_nWndHeight = rc.bottom - rc.top;
	m_pWindow = SDL_CreateWindowFrom(hWnd);
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!m_pRenderer)
		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_SOFTWARE);
	
	if (!m_pRenderer)
	{
		av_log(NULL, AV_LOG_ERROR, "Create Hardware or Software Render Failed!!!");
		SDL_DestroyWindow(m_pWindow);
		m_pWindow = nullptr;
		return false;
	}

	m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, m_nVideoWidth, m_nVideoHeight);

	if (pixelFormat != AV_PIX_FMT_YUV420P)
	{
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
	}

	return true;
}

void CSDLRender::DestoryRender()
{
	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	if (m_pWindow)
		SDL_DestroyWindow(m_pWindow);

	if (m_pRenderer)
		SDL_DestroyRenderer(m_pRenderer);

	if (m_pTexture)
		SDL_DestroyTexture(m_pTexture);
}

bool CSDLRender::SetRenderMode(RENDER_MODE)
{
	return false;
}

void CSDLRender::SetRenderSize(int width, int height)
{
	m_nWndWidth = width;
	m_nWndHeight = height;
	m_bSizeChanged = true;
}

void CSDLRender::RenderFrameData(FramePtr pFrame)
{
	AVFrame* pTempFrame;
	if (m_pSwsCtx)
	{
		sws_scale(
			m_pSwsCtx,
			(uint8_t const * const *)pFrame->data,
			pFrame->linesize,
			0,
			pFrame->height,
			m_pFrameOut->data,
			m_pFrameOut->linesize);
		pTempFrame = m_pFrameOut;
	}
	else
		pTempFrame = pFrame.get();

	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);

	int ret = 0;
	if (pTempFrame->linesize[0] > 0 && pTempFrame->linesize[1] > 0 && pTempFrame->linesize[2] > 0) {
		ret = SDL_UpdateYUVTexture(m_pTexture, NULL, pTempFrame->data[0], pTempFrame->linesize[0],
			pTempFrame->data[1], pTempFrame->linesize[1],
			pTempFrame->data[2], pTempFrame->linesize[2]);
	}
	else if (pTempFrame->linesize[0] < 0 && pTempFrame->linesize[1] < 0 && pTempFrame->linesize[2] < 0) {
		ret = SDL_UpdateYUVTexture(m_pTexture, NULL, pTempFrame->data[0] + pTempFrame->linesize[0] * (pTempFrame->height - 1), -pTempFrame->linesize[0],
			pTempFrame->data[1] + pTempFrame->linesize[1] * (AV_CEIL_RSHIFT(pTempFrame->height, 1) - 1), -pTempFrame->linesize[1],
			pTempFrame->data[2] + pTempFrame->linesize[2] * (AV_CEIL_RSHIFT(pTempFrame->height, 1) - 1), -pTempFrame->linesize[2]);
	}
	else {
		av_log(NULL, AV_LOG_ERROR, "Mixed negative and positive linesizes are not supported.\n");
		return;
	}
	//SDL_UpdateTexture(m_pTexture, NULL, pTempFrame->data[0], pTempFrame->linesize[0]);
	SDL_RenderClear(m_pRenderer);
	SDL_RenderCopyEx(m_pRenderer, m_pTexture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(m_pRenderer);
	int nWidth = 0, nHeight = 0;
	SDL_GetWindowSize(m_pWindow, &nWidth, &nHeight);
	if (m_bSizeChanged)
	{
		SDL_DestroyRenderer(m_pRenderer);
		SDL_DestroyTexture(m_pTexture);
		SDL_Rect sdl_rect;
		sdl_rect.x = 0;
		sdl_rect.y = 0;
		sdl_rect.w = m_nWndWidth;
		sdl_rect.h = m_nWndHeight;
		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
		m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, m_nVideoWidth, m_nVideoHeight);
		m_bSizeChanged = false;
	}
}

void CSDLRender::SetScale(float factor)
{
	/*if (m_pRenderer)
		SDL_RenderSetScale(m_pRenderer, factor, factor);*/
}

void CSDLRender::SetRotate(float x, float y)
{
}

void CSDLRender::SetScroll(float latitude, float longitude)
{
}

void CSDLRender::SetReverse(bool filp)
{
}
