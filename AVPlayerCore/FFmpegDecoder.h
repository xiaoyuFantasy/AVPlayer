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
	virtual int DecodeFrame(FramePtr& pFrame, PacketQueue& queue) override;
	virtual void SetRenderCallback(std::function<void(AVFrame*)> funcRender) override;

private:
	AVCodecContext*	m_pCodecCtx = nullptr;
	std::function<void(AVFrame* pFrame)> m_funcRender;
};