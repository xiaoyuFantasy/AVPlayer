#pragma once
#include "stdafx.h"

class IDecoder
{
public:
	IDecoder() = default;
	virtual ~IDecoder() = default;

public:
	virtual bool Init(AVCodecContext* ctx) = 0;
	virtual void UnInit() = 0;
	virtual int DecodeFrame(FramePtr& pFrame, PacketQueue& queue) = 0;
	virtual void SetRenderCallback(std::function<void(AVFrame* pFrame)> funcRender) = 0;
};