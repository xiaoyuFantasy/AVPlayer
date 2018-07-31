#pragma once
#include "stdafx.h"

enum RENDER_MODE {
	PANO2D = 0,		// ��ͨ2D����ģʽ
	STANDARD = 1,	// ����ȫ������ģʽ
	FISH_EYE = 2,	// ���۲���ģʽ
	STEREO = 3,		// С���ǲ���ģʽ
};

class IRender
{
public:
	virtual bool InitRender() = 0;
	virtual bool CreateRender(HWND hWnd) = 0;
	virtual void DestoryRender() = 0;
	virtual bool SetRenderMode(RENDER_MODE) = 0;
	virtual void SetRenderSize(int, int) = 0;
	virtual void RenderFrameData(AVFrame *frame) = 0;
	virtual void SetScale(float factor) = 0;
	virtual void SetRotate(float x, float y) = 0;
	virtual void SetScroll(float latitude, float longitude) = 0;
	virtual void SetReverse(bool filp) = 0;
};