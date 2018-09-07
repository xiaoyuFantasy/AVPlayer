#include "stdafx.h"
#include "D3DRender.h"
#pragma comment(lib, "D3D9.lib")

//Flexible Vertex Format, FVF
typedef struct
{
	FLOAT       x, y, z;
	FLOAT       rhw;
	D3DCOLOR    diffuse;
	FLOAT       tu, tv;
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

CD3DRender::CD3DRender()
{
	
}

CD3DRender::~CD3DRender()
{
}

bool CD3DRender::InitRender()
{
	CAutoLock lock(m_mutex);
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pDirect3D9)
		return false;

	D3DDISPLAYMODE d3dDisplayMode;
	HRESULT hRet = m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);
	av_log(NULL, AV_LOG_INFO, "screen resolution w:%d, h:%d", d3dDisplayMode.Width, d3dDisplayMode.Height);
	return true;
}

bool CD3DRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return -1;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	GetClientRect(hWnd, &m_rtViewport);

	HRESULT hRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDirect3DDevice);
	if (FAILED(hRet))
		return false;

	hRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		nWidth, nHeight,
		(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	if (FAILED(hRet))
		return false;

	m_hWnd = hWnd;
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	if (AV_PIX_FMT_YUV420P != pixelFormat )
	{
		//创建转换函数
		if (m_pFrameOut)
			av_frame_free(&m_pFrameOut);
		m_pFrameOut = av_frame_alloc();
		m_pFrameOut->format = AV_PIX_FMT_YUV420P;
		m_pFrameOut->width = nWidth;
		m_pFrameOut->height = nHeight;
		m_pSwsCtx = sws_getContext(
			nWidth,
			nHeight,
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

void CD3DRender::DestoryRender()
{
	CAutoLock lock(m_mutex);
	if (m_pDirect3DSurfaceRender)
		m_pDirect3DSurfaceRender->Release();
	if (m_pDirect3DDevice)
		m_pDirect3DDevice->Release();
	if (m_pDirect3D9)
		m_pDirect3D9->Release();
}

bool CD3DRender::SetRenderMode(RENDER_MODE)
{
	return false;
}

void CD3DRender::SetRenderSize(int width, int height)
{
}

void CD3DRender::RenderFrameData(FramePtr pFrame)
{
	if (!pFrame)
		return;
	
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
	}

	D3DLOCKED_RECT d3d_rect;
	HRESULT hRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(hRet))
		return;

	byte *pSrc = (byte*)pFrame->data[0];
	byte * pDest = (byte*)d3d_rect.pBits;

	int stride = d3d_rect.Pitch;
	int pixel_w_size = m_nVideoWidth * 4;
	/*for (int i = 0; i< m_nVideoHeight; i++) {
		OutputDebugString(std::to_wstring(i).c_str());
		OutputDebugString(L"\n");
		memcpy(pDest, pSrc, pixel_w_size);
		pDest += stride;
		pSrc += pixel_w_size;
	}*/

	/*for (int i = 0; i < pixel_h; i++) {
		memcpy(pDest + i * stride, pSrc + i * pixel_w, pixel_w);
	}
	for (int i = 0; i < pixel_h / 2; i++) {
		memcpy(pDest + stride * pixel_h + i * stride / 2, pSrc + pixel_w * pixel_h + pixel_w * pixel_h / 4 + i * pixel_w / 2, pixel_w / 2);
	}
	for (int i = 0; i < pixel_h / 2; i++) {
		memcpy(pDest + stride * pixel_h + stride * pixel_h / 4 + i * stride / 2, pSrc + pixel_w * pixel_h + i * pixel_w / 2, pixel_w / 2);*/


	hRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(hRet))
		return;

	if (m_pDirect3DDevice == NULL)
		return;

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;

	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	pBackBuffer->Release();
}

void CD3DRender::SetScale(float factor)
{
}

void CD3DRender::SetRotate(float x, float y)
{
}

void CD3DRender::SetScroll(float latitude, float longitude)
{
}

void CD3DRender::SetReverse(bool filp)
{
}
