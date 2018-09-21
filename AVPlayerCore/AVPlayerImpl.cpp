#include "stdafx.h"
#include "AVPlayerImpl.h"
#include "SoundFactory.h"
#include "RenderFactory.h"

CAVPlayerImpl::CAVPlayerImpl()
{
}


CAVPlayerImpl::~CAVPlayerImpl()
{
}

bool CAVPlayerImpl::Open(PLAYER_OPTS & opts, bool bSync)
{
	if (m_bOpened || m_bOpening)
		return false;

	m_statusPlayer = OpeningStatus;
	m_bStop = false;
	m_opts = opts;
	m_bOpening = true;
	m_threadOpen = std::thread([&]() {
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
			if (ret == 0)
				break;
			else if (ret == AVERROR_BUFFER_TOO_SMALL)
			{
				av_log(NULL, AV_LOG_ERROR, "avformat_open_input. failed!!! buffer too small ");
				continue;
			}
			else
			{
				char szErr[AV_ERROR_MAX_STRING_SIZE];
				av_strerror(ret, szErr, AV_ERROR_MAX_STRING_SIZE);
				av_log(NULL, AV_LOG_ERROR, "avformat_open_input. err_code:%d, msg:%s", ret, szErr);
				break;
			}
		}

		if (ret != 0)
		{
			av_log(NULL, AV_LOG_ERROR, "can not open the file. err_code:%d", ret);
			goto OPEN_ERROR;
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
			m_pSound->InitAudio();
			m_audioPlayer.SetSound(m_pSound);
			m_audioPlayer.SetStream(m_pFormatCtx->streams[m_nAudioIndex]);
			m_audioPlayer.SetClockMgr(&m_clockMgr);
			if (m_audioPlayer.Open())
				m_bAudioOpen = true;
		}

		if (m_nVideoIndex != -1 && m_opts.bEnableVideo)
		{
			if (!m_pRender)
				m_pRender = CRenderFactory::getSingleModule().CreateRender("opengl");
			m_pRender->InitRender();
			if (1 == m_opts.video_type)
				m_pRender->SetRenderMode(RENDER_MODE::PANO2D);
			else
				m_pRender->SetRenderMode(RENDER_MODE::STANDARD);
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

		m_statusPlayer = OpenedStatus;
		PlayerOpenSt st;
		st.duration = m_duration;
		st.IsHasAudio = m_bAudioOpen;
		st.IsHasVideo = m_bVideoOpen;
		CallbackEvent(PlayerOpening, &st);
		m_bOpened = true;
		m_bOpening = false;
		return true;

	OPEN_ERROR:
		avformat_close_input(&m_pFormatCtx);
		if (m_pFormatCtx)
			avformat_free_context(m_pFormatCtx);

		PlayerErrorSt err;
		err.nCode = ret;
		err.strErrMsg = "open input failed!!! url:" + m_opts.strPath;
		CallbackEvent(PlayerError, &err);
		m_bOpening = false;
		return false;
	});

	return true;
}

bool CAVPlayerImpl::Play()
{
	if (m_bPlaying || !m_bOpened)
		return false;
	m_bPlaying = true;
	m_statusPlayer = PlayingStatus;

	if (m_bAudioOpen)
		m_pSound->Start();

	if (m_bVideoOpen)
		m_videoPlayer.Play();

	m_threadPlay = std::thread([&]() {
		while (!m_bStop)
		{
			if (m_bPause)
			{
				av_read_pause(m_pFormatCtx);
				std::unique_lock<std::mutex> lock(m_mxPauseWait);
				m_cvPause.wait(lock);
				av_read_play(m_pFormatCtx);
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

			PacketPtr packet{ av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); } };
			av_init_packet(packet.get());
			int ret = av_read_frame(m_pFormatCtx, packet.get());
			if (ret < 0)
			{
				if (ret == AVERROR_EOF || avio_feof(m_pFormatCtx->pb))
					av_log(NULL, AV_LOG_ERROR, "av_read_frame failed eof error!!!");
				std::this_thread::sleep_for(std::chrono::seconds(1));

				PlayerErrorSt err;
				err.nCode = ret;
				err.strErrMsg = "read frame failed!!! url:" + m_opts.strPath;
				CallbackEvent(PlayerError, &err);
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

	return true;
}

void CAVPlayerImpl::Pause()
{
	m_bPause = true;
	m_statusPlayer = PausedStatus;
}

void CAVPlayerImpl::Resume()
{
	m_bPause = false;
	m_cvPause.notify_all();
	m_statusPlayer = PlayingStatus;
}

void CAVPlayerImpl::Stop()
{
	m_statusPlayer = StopedStatus;
	Close();
}

void CAVPlayerImpl::SetRenderMode(RENDER_MODE mode)
{
	if (m_bVideoOpen && m_pRender)
		m_pRender->SetRenderMode(mode);
}

PLAY_STATUS CAVPlayerImpl::Status()
{
	return m_statusPlayer;
}

bool CAVPlayerImpl::WaitForCompletion()
{
	while (m_statusPlayer != NoneStatus)
		av_usleep(1000);

	return true;
}

void CAVPlayerImpl::Close()
{
	m_bStop = true;
	m_cvPause.notify_all();

	if (m_threadOpen.joinable())
		m_threadOpen.join();

	if (m_threadPlay.joinable())
		m_threadPlay.join();

	m_audioPlayer.Close();
	m_videoPlayer.Stop();

	avformat_close_input(&m_pFormatCtx);
	if (m_pFormatCtx)
		avformat_free_context(m_pFormatCtx);

	m_statusPlayer = NoneStatus;
	m_bOpened = false;

	if (m_opts.funcEvent)
		m_opts.funcEvent(m_opts.user_data, PlayerClosed, nullptr);
}

void CAVPlayerImpl::SeekTo(double fact)
{
	m_nPos = fact;
	m_bSeek = true;
}

void CAVPlayerImpl::Volume(int nVolume)
{
	if (m_pSound)
		m_pSound->SetVolume(nVolume);
}

void CAVPlayerImpl::Mute(bool s)
{
	
}

void CAVPlayerImpl::FullScreen(bool bfull)
{
}

double CAVPlayerImpl::CurrentPlayTime()
{
	return 0.0;
}

double CAVPlayerImpl::Duration()
{
	return 0.0;
}

void CAVPlayerImpl::VideoSize(int & width, int & height)
{
	if (m_bVideoOpen && m_pRender)
		m_pRender->SetRenderSize(width, height);
}

double CAVPlayerImpl::Buffering()
{
	return 0.0;
}

void CAVPlayerImpl::SetScale(float factor)
{
	if (m_bVideoOpen && m_pRender)
		m_pRender->SetScale(factor);
}

void CAVPlayerImpl::SetRotate(double xoffset, double yoffset)
{
	if (m_bVideoOpen && m_pRender)
		m_pRender->SetRotate(xoffset, yoffset);
}

int CAVPlayerImpl::interrupt_callback(void * lparam)
{
	CAVPlayerImpl *p = (CAVPlayerImpl*)lparam;
	if (p->m_bStop)
		return 1;
	return 0;
}

void CAVPlayerImpl::CallbackEvent(PLAYER_EVENT e, LPVOID lpData)
{
	CAutoLock lock(m_mutexEvent);
	if (m_opts.funcEvent)
		m_opts.funcEvent(m_opts.user_data, e, lpData);
}

