#include "stdafx.h"
#include "FFmpegDecoder.h"
#include <d3d9.h>

typedef struct DXVA2DevicePriv {
	HMODULE d3dlib;
	HMODULE dxva2lib;

	HANDLE device_handle;

	IDirect3D9       *d3d9;
	IDirect3DDevice9 *d3d9device;
} DXVA2DevicePriv;


CFFmpegDecoder::CFFmpegDecoder()
{
}

CFFmpegDecoder::~CFFmpegDecoder()
{
}

bool CFFmpegDecoder::Init(AVCodecContext * ctx)
{
	m_pCodecCtx = ctx;
	return true;
}

void CFFmpegDecoder::UnInit()
{
	m_pCodecCtx = nullptr;
}

int CFFmpegDecoder::DecodeFrame(FramePtr& pFrame, PacketQueue & queue)
{
	if (!m_pCodecCtx || !pFrame)
		return -1;

	int ret = 0;
	for (;;)
	{
		do
		{
			ret = avcodec_receive_frame(m_pCodecCtx, pFrame.get());
			if (ret == AVERROR_EOF)
			{
				avcodec_flush_buffers(m_pCodecCtx);
				return -1;
			}

			if (ret >= 0)
			{
				if (m_pCodecCtx->codec_type == AVMEDIA_TYPE_AUDIO)
				{
					AVRational tb{ 1, pFrame->sample_rate };
					if (pFrame->pts == AV_NOPTS_VALUE)
						pFrame->pts = av_rescale_q(pFrame->pts, m_pCodecCtx->pkt_timebase, tb) + pFrame->pkt_duration;
				}
				else if (m_pCodecCtx->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					//
				}
				return TRUE;
			}

		} while (ret != AVERROR(EAGAIN));

		PacketPtr packet{ nullptr, [](AVPacket* p) { av_packet_free(&p); } };
		if (queue.Pop(packet))
		{
			if (avcodec_send_packet(m_pCodecCtx, packet.get()) == AVERROR(EAGAIN))
			{
				char szErr[512] = { 0 };
				av_strerror(ret, szErr, sizeof(szErr));
				av_log(NULL, AV_LOG_ERROR, "video codec send packet error. err:%d, %s", ret, szErr);
			}
		}
		else
			return ret;
	}

	return ret;
}