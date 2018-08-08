#include "stdafx.h"
#include "PlayerImpl.h"
#include "SoundFactory.h"
#include "RenderFactory.h"

CPlayerImpl::CPlayerImpl()
{
}


CPlayerImpl::~CPlayerImpl()
{
}

bool CPlayerImpl::Open(PLAYER_OPTS & opts, bool bSync = true)
{
	if (m_bOpened || m_bOpening)
		return false;

	m_bOpening = true;
	bool bOpen = false;
	std::packaged_task<bool()> task([&]() {
		if (m_pFormatCtx)
			avformat_free_context(m_pFormatCtx);

		m_pFormatCtx = avformat_alloc_context();
		AVIOInterruptCB cb = { interrupt_callback , this };
		m_pFormatCtx->interrupt_callback = cb;
		m_pFormatCtx->flush_packets = 1;
		m_pFormatCtx->max_analyze_duration = 1 * AV_TIME_BASE;
		int nIndex = 0;
		int nMaxCount = 10;
		int ret = 0;
		for (nIndex = 0; nIndex < nMaxCount; nIndex++)
		{
			m_pFormatCtx->probesize = 1 * 1024 * (nIndex + 1);
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
			goto OPEN_ERROR;
		}

		for (size_t i = 0; i < m_pFormatCtx->nb_streams; i++)
		{
			if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
				m_nAudioIndex = i;

			if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				m_nVideoIndex = i;
		}

		if (m_nAudioIndex != -1 && m_opts.bEnableAudio)
		{
			if (!m_pSound)
				m_pSound = CSoundFactory::getSingleModule().CreateSound();
			m_audioPlayer.SetSound(m_pSound);
			m_audioPlayer.SetStream(m_pFormatCtx->streams[m_nAudioIndex]);
			m_audioPlayer.SetClockMgr(&m_clockMgr);
			if (m_audioPlayer.Open())
				m_bAudioOpen = true;
		}

		if (m_nVideoIndex != -1 && m_opts.bEnableVideo)
		{
			if (!m_pRender)
				m_pRender = CRenderFactory::getSingleModule().CreateRender();
			m_videoPlayer.SetRender(m_pRender);
			m_videoPlayer.SetStream(m_pFormatCtx->streams[m_nVideoIndex]);
			m_videoPlayer.SetClockMgr(&m_clockMgr);
			if (m_videoPlayer.Open(m_opts))
				m_bVideoOpen = true;
		}

		if (!m_bAudioOpen && !m_bVideoOpen)
		{
			av_log(NULL, AV_LOG_ERROR, "can not open video and audio player.");
			goto OPEN_ERROR;
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
		m_bOpened = true;
		m_bOpening = false;
		m_statusPlayer = OpenedStatus;
		return true;

	OPEN_ERROR:
		avformat_close_input(&m_pFormatCtx);
		if (m_pFormatCtx)
			avformat_free_context(m_pFormatCtx);
		return false;
	});

	std::future<bool> ret;
	if (bSync)
		ret = task.get_future();

	std::thread(std::move(task)).detach();
	if (bSync && ret.valid())
		bOpen = ret.get();

	return bOpen;
}

bool CPlayerImpl::Play()
{
	if (m_bPlaying)
		return false;

	m_bPlaying = true;
	m_statusPlayer = PlayingStatus;
	m_threadPlay = std::thread([&]() {
		while (!m_bStop)
		{
			if (m_bPause)
			{
				std::unique_lock<std::mutex> lock(m_mxPauseWait);
				m_cvPause.wait(lock);
			}

			if (m_bSeek)
			{
				int64_t target = m_nPos*AV_TIME_BASE;
				int ret = avformat_seek_file(m_pFormatCtx, -1, INT64_MIN, target, INT64_MAX, 0);
				if (ret < 0)
					av_log(NULL, AV_LOG_ERROR, "avformat_seek_file Failed !!! ts:%lld", target);
				else
				{
					if (m_bAudioOpen)
						m_audioPlayer.Reset();

					if (m_bVideoOpen)
						m_videoPlayer.Reset();
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
				if (ret == AVERROR_EOF || avio_feof(m_pFormatCtx->pb))
					av_log(NULL, AV_LOG_ERROR, "av_read_frame failed eof error!!!");
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
				continue;
			}

			if (m_bStop)
				break;

			if (m_bAudioOpen && packet->stream_index == m_nAudioIndex)
				m_audioPlayer.PushPacket(move(packet));
			else if (m_bVideoOpen && packet->stream_index == m_nVideoIndex)
				m_videoPlayer.PushPacket(move(packet));

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		m_bPlaying = false;
	});
	return false;
}

void CPlayerImpl::Pause()
{
	m_bPause = true;
	m_statusPlayer = PausedStatus;
}

void CPlayerImpl::Resume()
{
	m_bPause = false;
	m_cvPause.notify_all();
	m_statusPlayer = PlayingStatus;
}

void CPlayerImpl::Stop()
{
	Close();
}

bool CPlayerImpl::WaitForCompletion()
{
	while (m_statusPlayer != CloseStatus)
		av_usleep(10000);

	return true;
}

void CPlayerImpl::Close()
{
	m_bStop = true;
	m_cvPause.notify_all();
	if (m_threadPlay.joinable())
		m_threadPlay.join();

	m_audioPlayer.Close();
	m_videoPlayer.Stop();

	CSoundFactory::getSingleModule().ReleaseSound(&m_pSound);
	CRenderFactory::getSingleModule().ReleaseRender(&m_pRender);

	avformat_close_input(&m_pFormatCtx);
	if (m_pFormatCtx)
		avformat_free_context(m_pFormatCtx);
}

void CPlayerImpl::SeekTo(double fact)
{
	m_nPos = fact;
	m_bSeek = true;
}

void CPlayerImpl::Volume(int nVolume)
{
	if (m_bAudioOpen && m_pSound)
		m_pSound->SetVolume(nVolume);
}

void CPlayerImpl::Mute(bool s)
{
	
}

void CPlayerImpl::FullScreen(bool bfull)
{
}

double CPlayerImpl::CurrentPlayTime()
{
	return 0.0;
}

double CPlayerImpl::Duration()
{
	return 0.0;
}

bool CPlayerImpl::VideoSize(int & width, int & height)
{
	return false;
}

double CPlayerImpl::Buffering()
{
	return 0.0;
}

int CPlayerImpl::interrupt_callback(void * lparam)
{
	CPlayerImpl *p = (CPlayerImpl*)lparam;
	if (p->m_bStop)
		return 1;
	return 0;
}
