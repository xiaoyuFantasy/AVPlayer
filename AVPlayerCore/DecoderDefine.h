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
	virtual int DecodeFrame(AVFrame* frame, PacketQueue& queue) = 0;
};