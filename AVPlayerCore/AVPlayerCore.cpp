// AVPlayerCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AVPlayerCore.h"

CAVPlayerCore::CAVPlayerCore(PLAYER_OPTS & opts)
	:m_opts(opts)
{
	m_bPlaying = false;
}

CAVPlayerCore::~CAVPlayerCore()
{
}

const char * CAVPlayerCore::GetPath()
{
	return nullptr;
}

bool CAVPlayerCore::Play()
{
	if (m_bPlaying)
		return false;

	m_bQuit = false;
	m_bPlaying = true;
	if (m_pFormatCtx)
		avformat_free_context(m_pFormatCtx);

	m_pFormatCtx = avformat_alloc_context();
	AVIOInterruptCB cb = { interrupt_callback , this };
	int nIndex = 0;
	int nMaxCount = 10;
	int ret = 0;
	for (nIndex = 0; nIndex < nMaxCount; nIndex++)
	{
		m_pFormatCtx->probesize = 10 * 1024 * (nIndex + 1);
		ret = avformat_open_input(&m_pFormatCtx, m_opts.strPath.c_str(), nullptr, nullptr);
		if (ret != AVERROR_BUFFER_TOO_SMALL)
			break;
	}

	if (ret != 0)
	{
		av_log(NULL, AV_LOG_ERROR, "can not open the file. err_code:%d", ret);
		return false;
	}

	if ((ret = avformat_find_stream_info(m_pFormatCtx, nullptr)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "can not find stream info. err_code:%d", ret);
		goto LOAD_ERROR;
	}

	for (size_t i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_nAudioIndex = i;
		}

		if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_nVideoIndex = i;
		}
	}

	if (m_nAudioIndex != -1 && m_opts.bEnableAudio)
	{
		m_pAudioPlayer = new CAudioPlayer(m_pFormatCtx->streams[m_nAudioIndex]);
		if (m_pAudioPlayer->Open(m_opts))
			m_bAudioOpen = true;
		else
		{
			delete m_pAudioPlayer;
			m_pAudioPlayer = nullptr;
		}
	}

	if (m_nVideoIndex != -1 && m_opts.bEnableVideo)
	{
		m_pVideoPlayer = new CVideoPlayer(m_pFormatCtx->streams[m_nVideoIndex]);
		if (m_pVideoPlayer->Open(m_opts))
		{
			if (m_bAudioOpen)
				m_pVideoPlayer->SetAudioClock(m_pAudioPlayer);
			m_bVideoOpen = true;
		}
		else
		{
			delete m_pVideoPlayer;
			m_pVideoPlayer = nullptr;
		}	
	}

	if (!m_bAudioOpen && !m_bVideoOpen)
	{
		av_log(NULL, AV_LOG_ERROR, "can not open video and audio player.");
		goto LOAD_ERROR;
	}
	
	if (m_pFormatCtx->duration != AV_NOPTS_VALUE)
		m_duration = m_pFormatCtx->duration + 5000;
	else
	{
		if (m_bVideoOpen)
			m_duration = m_pFormatCtx->streams[m_nVideoIndex]->duration * av_q2d(m_pFormatCtx->streams[m_nVideoIndex]->time_base);
		else
			m_duration = m_pFormatCtx->streams[m_nAudioIndex]->duration * av_q2d(m_pFormatCtx->streams[m_nAudioIndex]->time_base);
	}

	if (m_opts.durationCb)
		m_opts.durationCb(m_opts.user_data, m_duration / AV_TIME_BASE);

	{
		int hours, mins, secs, us;
		secs = m_duration / AV_TIME_BASE;
		us = m_duration % AV_TIME_BASE;
		av_log(NULL, AV_LOG_INFO, "duration: %ld, seconds: %ld", m_duration, secs);
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
	}

	if (m_bAudioOpen)
		m_pAudioPlayer->Play();

	if (m_bVideoOpen)
		m_pVideoPlayer->Play();

	while (!m_bQuit)
	{
		if (m_bPaused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		if (m_bSeek)
		{
			int64_t target = m_nPos*AV_TIME_BASE;
			int ret = avformat_seek_file(m_pFormatCtx, -1, INT64_MIN, target, INT64_MAX, 0);
			if (ret < 0)
				av_log(NULL, AV_LOG_ERROR, "avformat_seek_file Failed !!! ts:%lld", target);
			else
			{
				m_pAudioPlayer->ClearFrame();
				m_pVideoPlayer->ClearFrame();
			}
			av_log(NULL, AV_LOG_INFO, "avformat_seek_file");
			m_bSeek = false;
		}

		std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> packet{ av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); } };
		av_init_packet(packet.get());
		packet->data = nullptr;

		int ret = av_read_frame(m_pFormatCtx, packet.get());
		if (ret < 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
			continue;
		}

		if (m_bQuit)
			break;

		if (m_bAudioOpen && packet->stream_index == m_nAudioIndex)
			m_pAudioPlayer->PushPacket(move(packet));
		else if (m_bVideoOpen && packet->stream_index == m_nVideoIndex)
		{
			double video_pts = packet->pts;
			video_pts *= av_q2d(m_pFormatCtx->streams[m_nVideoIndex]->time_base);
			//av_log(NULL, AV_LOG_INFO, "video pts:%f", video_pts);
			m_pVideoPlayer->PushPacket(move(packet));
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	m_bQuit = true;

	if (m_bAudioOpen && m_pAudioPlayer)
		m_pAudioPlayer->Stop();

	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->Stop();

LOAD_ERROR:
	avformat_close_input(&m_pFormatCtx);
	if (m_pFormatCtx)
		avformat_free_context(m_pFormatCtx);

	m_bPlaying = false;
	return false;
}

bool CAVPlayerCore::IsPlaying() const
{
	if (m_pVideoPlayer)
		return m_pVideoPlayer->IsPlaying();

	if (m_pAudioPlayer)
		return m_pAudioPlayer->IsPlaying();

	return false;
}

void CAVPlayerCore::Pause(bool bPause)
{
	m_bPaused = bPause;
	if (m_pVideoPlayer)
		m_pVideoPlayer->Pause(bPause);
	if (m_pAudioPlayer)
		m_pAudioPlayer->Pause(bPause);
}

bool CAVPlayerCore::IsPaused() const
{
	return m_bPaused;
}

void CAVPlayerCore::Stop()
{
	m_bQuit = true;
}

void CAVPlayerCore::Seek(int nPos)
{
	if (m_bPlaying)
	{
		m_bSeek = true;
		m_nPos = nPos;
	}
}

void CAVPlayerCore::SetVolume(int nVolume)
{
	if (m_pAudioPlayer)
		m_pAudioPlayer->SetVolume(nVolume);
}

int CAVPlayerCore::GetVolume() const
{
	if (m_pAudioPlayer)
		return m_pAudioPlayer->GetVolume();
	return 0;
}

void CAVPlayerCore::SetMuted(bool bMuted)
{
	if (m_pAudioPlayer)
		m_pAudioPlayer->SetMuted(bMuted);
}

bool CAVPlayerCore::IsMuted() const
{
	if (m_pAudioPlayer)
		return m_pAudioPlayer->IsMuted();
	return true;
}

void CAVPlayerCore::SetPanormaicType(PLAY_MODE type)
{
	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->SetPanormaicType(type);
}

void CAVPlayerCore::SetPanoramicScale(float factor)
{
	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->SetPanoramicScale(factor);
}

void CAVPlayerCore::SetPanoramicRotate(float x, float y)
{
	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->SetPanoramicRotate(x, y);
}

void CAVPlayerCore::SetPanoramicScroll(float latitude, float longitude)
{
	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->SetPanoramicScroll(latitude, longitude);
}

void CAVPlayerCore::SetWindowSize(int nWidth, int nHeight)
{
	if (m_bVideoOpen && m_pVideoPlayer)
		m_pVideoPlayer->SetWindowSize(nWidth, nHeight);
}

int CAVPlayerCore::interrupt_callback(void * lparam)
{
	CAVPlayerCore *p = (CAVPlayerCore*)lparam;
	if (p->m_bQuit)
		return 1;
	return 0;
}
