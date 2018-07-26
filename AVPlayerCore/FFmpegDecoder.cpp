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

int CFFmpegDecoder::DecodeFrame(AVFrame * frame, PacketQueue & queue)
{
	int ret = AVERROR(EAGAIN);
	FramePtr pTempFrame{ av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); } };
	for (;;)
	{
		do
		{
			switch (m_pCodecCtx->codec_type)
			{
			case AVMEDIA_TYPE_AUDIO:
				ret = avcodec_receive_frame(m_pCodecCtx, pTempFrame.get());
				if (ret >= 0)
				{
					AVRational tb{ 1, frame->sample_rate };
					if (pTempFrame->pts != AV_NOPTS_VALUE)
						pTempFrame->pts = av_rescale_q(pTempFrame->pts, m_pCodecCtx->pkt_timebase, tb);
					frame = std::move(pTempFrame.get());
				}
				break;
			case AVMEDIA_TYPE_VIDEO:
				ret = avcodec_receive_frame(m_pCodecCtx, pTempFrame.get());
				if (ret >= 0)
				{
					pTempFrame->pts = pTempFrame->best_effort_timestamp;
					if (m_pCodecCtx->hwaccel_context)
					{
						if ((ret = av_hwframe_transfer_data(frame, pTempFrame.get(), 0)) < 0)
						{
							av_log(NULL, AV_LOG_ERROR, "Error transferring the data to system memory. err:%d", ret);
							continue;
						}

						frame->pts = pTempFrame->pts;
						frame->pkt_dts = pTempFrame->pkt_dts;
						frame->pkt_pos = pTempFrame->pkt_pos;
						frame->pkt_duration = pTempFrame->pkt_duration;
						frame->pkt_size = pTempFrame->pkt_size;
						frame->best_effort_timestamp = pTempFrame->best_effort_timestamp;
						frame->repeat_pict = pTempFrame->repeat_pict;
					}
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
