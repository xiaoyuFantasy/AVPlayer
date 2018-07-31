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

bool CSDLRender::CreateRender(HWND hWnd)
{
	if (!::IsWindow(hWnd))
		return false;

	if (m_pWindow)
		return false;

	RECT rc;
	GetWindowRect(hWnd, &rc);
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

	m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, m_nWndWidth, m_nWndHeight);
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
	SDL_SetWindowSize(m_pWindow, width, height);
}

void CSDLRender::RenderFrameData(AVFrame *frame)
{
	SDL_Rect sdl_rect;
	sdl_rect.x = 0;
	sdl_rect.y = 0;
	sdl_rect.w = m_nWndWidth;
	sdl_rect.h = m_nWndHeight;
	SDL_UpdateTexture(m_pTexture, NULL, frame->data[0], frame->linesize[0]);
	SDL_RenderClear(m_pRenderer);
	SDL_RenderCopy(m_pRenderer, m_pTexture, NULL, &sdl_rect);
	SDL_RenderPresent(m_pRenderer);
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
