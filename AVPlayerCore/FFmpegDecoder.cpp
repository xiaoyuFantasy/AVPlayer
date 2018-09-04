#include "stdafx.h"
#include "FFmpegDecoder.h"


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

	int ret = AVERROR(EAGAIN);
	FramePtr pTempFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
	for (;;)
	{
		do
		{
			switch (m_pCodecCtx->codec_type)
			{
			case AVMEDIA_TYPE_AUDIO:
				ret = avcodec_receive_frame(m_pCodecCtx, pFrame.get());
				if (ret >= 0)
				{
					AVRational tb{ 1, pFrame->sample_rate };
					if (pFrame->pts == AV_NOPTS_VALUE)
						pFrame->pts = av_rescale_q(pFrame->pts, m_pCodecCtx->pkt_timebase, tb) + pFrame->pkt_duration;
				}
				break;
			case AVMEDIA_TYPE_VIDEO:
				ret = avcodec_receive_frame(m_pCodecCtx, pTempFrame.get());
				if (ret >= 0)
				{
					// �ж��Ƿ���Ӳ��
					if (m_pCodecCtx->hw_device_ctx)
					{
						if ((ret = av_hwframe_transfer_data(pFrame.get(), pTempFrame.get(), 0)) < 0)
						{
							av_log(NULL, AV_LOG_ERROR, "Error transferring the data to system memory. err:%d", ret);
							continue;
						}
						av_frame_copy_props(pFrame.get(), pTempFrame.get());
					}
					else
						pFrame = std::move(pTempFrame);
				}
				break;
			default:
				break;
			}
			
			if (ret == AVERROR_EOF)
			{
				avcodec_flush_buffers(m_pCodecCtx);
				return -1;
			}

			if (ret >= 0)
				return 1;
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
	}
	return 0;
}
