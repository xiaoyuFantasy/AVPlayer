#include "stdafx.h"
#include "D3DRender.h"
#include <d3d9.h>
#include <Dxva2api.h>

typedef IDirect3D9* WINAPI pDirect3DCreate9(UINT);
typedef HRESULT WINAPI pCreateDeviceManager9(UINT *, IDirect3DDeviceManager9 **);

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
	m_hD3d = LoadLibrary(L"d3d9.dll");
	if (!m_hD3d) {
		av_log(NULL, AV_LOG_ERROR, "failed to load D3D9 library\n");
		return false;
	}

	pDirect3DCreate9  *createD3D = (pDirect3DCreate9 *)GetProcAddress(m_hD3d, "Direct3DCreate9");
	if (!createD3D) {
		av_log(NULL, AV_LOG_ERROR, "failed to locate Direct3DCreate9\n");
		return false;
	}

	m_pDirect3D9 = createD3D(D3D_SDK_VERSION);
	if (!m_pDirect3D9) {
		av_log(NULL, AV_LOG_ERROR, "failed to create IDirect3D object\n");
		return false;
	}

	return true;
}

bool CD3DRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	D3DDISPLAYMODE d3ddm;
	UINT adapter = D3DADAPTER_DEFAULT;
	IDirect3D9_GetAdapterDisplayMode(m_pDirect3D9, adapter, &d3ddm);
	
	D3DCAPS9 caps;
	DWORD BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	HRESULT hr = m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	if (SUCCEEDED(hr))
	{
		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;// | D3DCREATE_FPU_PRESERVE;
		else
			BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;// | D3DCREATE_FPU_PRESERVE;
	}

	if (m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = nullptr;
	}

	m_d3dpp.Windowed = TRUE;
	m_d3dpp.hDeviceWindow = hWnd;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.BackBufferWidth = nWidth;
	m_d3dpp.BackBufferHeight = nHeight;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = d3ddm.Format;
	m_d3dpp.EnableAutoDepthStencil = FALSE;
	m_d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	hr = IDirect3D9_CreateDevice(m_pDirect3D9, adapter, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &m_d3dpp, &m_pDirect3DDevice);
	if (FAILED(hr)) {
		av_log(NULL, AV_LOG_ERROR, "Failed to create Direct3D device\n");
		return false;
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

bool CD3DRender::SetRenderMode(AV_RENDER_MODE)
{
	return false;
}

void CD3DRender::SetRenderSize(int width, int height)
{
}

void CD3DRender::RenderFrameData(AVFrame *pFrame)
{
	if (pFrame->format != AV_PIX_FMT_DXVA2_VLD)
		return;

	LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9)pFrame->data[3];
	if (!surface)
	{
		av_log(NULL, AV_LOG_ERROR, "frame data[3] to LPDIRECT3DSURFACE9 failed!!!");
		return;
	}

	CAutoLock lock(m_mutex);
	if (m_pDirect3DSurfaceRender)
	{
		m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;
	}

	m_pDirect3DDevice->Reset(&m_d3dpp);
	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pDirect3DSurfaceRender);
	m_pDirect3DDevice->StretchRect(surface, NULL, m_pDirect3DSurfaceRender, NULL, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);

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
