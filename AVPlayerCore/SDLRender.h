#pragma once
#include "RenderDefine.h"

class CSDLRender : public IRender
{
public:
	CSDLRender();
	virtual ~CSDLRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd) override;
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
	SDL_Window*		m_pWindow = nullptr;
	SDL_Renderer*	m_pRenderer = nullptr;
	SDL_Texture*	m_pTexture = nullptr;
};

