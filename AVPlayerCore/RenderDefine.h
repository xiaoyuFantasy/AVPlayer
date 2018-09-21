#pragma once
#include "stdafx.h"
#include "AVPlayerDefine.h"


class IRender
{
public:
	virtual bool InitRender() = 0;
	virtual bool CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat = 0) = 0;
	virtual void DestoryRender() = 0;
	virtual bool SetRenderMode(RENDER_MODE) = 0;
	virtual void SetRenderSize(int, int) = 0;
	virtual void RenderFrameData(AVFrame *pFrame) = 0;
	virtual void SetScale(float factor) = 0;
	virtual void SetRotate(float x, float y) = 0;
	virtual void SetScroll(float latitude, float longitude) = 0;
	virtual void SetReverse(bool filp) = 0;
};