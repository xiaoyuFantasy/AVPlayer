#pragma once
#include "RenderDefine.h"

class CSDLRender : public IRender
{
public:
	CSDLRender();
	virtual ~CSDLRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat = AV_PIX_FMT_YUV420P) override;
	void DestoryRender() override;
	bool SetRenderMode(RENDER_MODE) override;
	void SetRenderSize(int width, int height) override;
	void RenderFrameData(AVFrame *frame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

private:
	int		m_nWndWidth = 0;
	int		m_nWndHeight = 0;
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	std::atomic_bool m_bSizeChanged = false;
	SDL_Window*		m_pWindow = nullptr;
	SDL_Renderer*	m_pRenderer = nullptr;
	SDL_Texture*	m_pTexture = nullptr;
	// 视频宽.
	int m_nVideoWidth = 0;
	// 视频高.
	int m_nVideoHeight = 0;
	//转换
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

