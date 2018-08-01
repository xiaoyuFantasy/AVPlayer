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

bool CSDLRender::CreateRender(HWND hWnd, int nWidth, int nHeight)
{
	if (!::IsWindow(hWnd))
		return false;

	if (m_pWindow)
		return false;

	RECT rc;
	::GetWindowRect(hWnd, &rc);
	m_nWndWidth = rc.right - rc.left;
	m_nWndHeight = rc.bottom - rc.top;
	m_pWindow = SDL_CreateWindowFrom(hWnd);
	//SDL_SetWindowBordered(m_pWindow, SDL_FALSE);
	//SDL_SetWindowResizable(m_pWindow, SDL_FALSE);
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

	m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, nWidth, nHeight);
	return true;
}

void CSDLRender::DestoryRender()
{
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

void CSDLRender::RenderFrameData(AVFrame *frame)
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_UpdateTexture(m_pTexture, NULL, frame->data[0], frame->linesize[0]);
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
		m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, frame->width, frame->height);
		m_bSizeChanged = false;
	}
}

void CSDLRender::SetScale(float factor)
{
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
