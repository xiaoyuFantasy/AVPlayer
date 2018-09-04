#pragma once
#include "RenderDefine.h"
#include <d3d9.h>

class CD3DRender : public IRender
{
public:
	CD3DRender();
	virtual ~CD3DRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat = AV_PIX_FMT_YUV420P) override;
	void DestoryRender() override;
	bool SetRenderMode(RENDER_MODE) override;
	void SetRenderSize(int width, int height) override;
	void RenderFrameData(FramePtr pFrame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

protected:


private:
	HWND					m_hWnd = nullptr;
	int						m_nVideoWidth = 0;
	int						m_nVideoHeight = 0;
	RECT                    m_rtViewport;
	std::mutex				m_mutex;
	IDirect3D9 *			m_pDirect3D9 = NULL;
	IDirect3DDevice9 *		m_pDirect3DDevice = NULL;
	IDirect3DSurface9 *		m_pDirect3DSurfaceRender = NULL;
	//×ª»»
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

