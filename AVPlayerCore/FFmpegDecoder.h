#pragma once
#include "DecoderDefine.h"
#include "Queue.h"

class CFFmpegDecoder : public IDecoder
{
public:
	CFFmpegDecoder();
	virtual ~CFFmpegDecoder();

public:
	virtual bool Init(AVCodecContext* ctx) override;
	virtual void UnInit() override;
	virtual int DecodeFrame(AVFrame* frame, PacketQueue& queue) override;

private:
	AVCodecContext*	m_pCodecCtx = nullptr;
};