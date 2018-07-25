#include "stdafx.h"
#include "AVPlayer.h"


CAVPlayer::CAVPlayer()
{
}


CAVPlayer::~CAVPlayer()
{
}

void CAVPlayer::Play(PLAYER_OPTS opts)
{
	av_log(NULL, AV_LOG_INFO, "AVPlayer");
	av_log(NULL, AV_LOG_INFO, "Path: %s", opts.strPath.c_str());
	
	std::thread([](PLAYER_OPTS opts, CAVPlayerCore **lpPlayerCore) {
		CAVPlayerCore* pCore = new CAVPlayerCore(opts);
		*lpPlayerCore = pCore;
		if (pCore)
		{
			pCore->Play();
			delete pCore;
			pCore = nullptr;
		}
	}, opts, &m_pPlayerCore).detach();
}

void CAVPlayer::Stop()
{
	if (m_pPlayerCore)
	{
		m_pPlayerCore->Stop();
		m_pPlayerCore = nullptr;
	}
}

bool CAVPlayer::IsPlaying() const
{
	if (m_pPlayerCore)
		return m_pPlayerCore->IsPlaying();

	return false;
}

void CAVPlayer::Pause(bool bPause)
{
	if (m_pPlayerCore)
		m_pPlayerCore->Pause(bPause);
}

bool CAVPlayer::IsPaused() const
{
	if (m_pPlayerCore)
		return m_pPlayerCore->IsPaused();

	return false;
}

void CAVPlayer::Seek(int nPos)
{
	if (m_pPlayerCore)
		m_pPlayerCore->Seek(nPos);
}

void CAVPlayer::SetVolume(int nVolume)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetVolume(nVolume);
}

int CAVPlayer::GetVolume() const
{
	if (m_pPlayerCore)
		return m_pPlayerCore->GetVolume();
	
	return 0;
}

void CAVPlayer::SetMuted(bool bMuted)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetMuted(bMuted);
}

bool CAVPlayer::IsMuted() const
{
	if (m_pPlayerCore)
		return m_pPlayerCore->IsMuted();

	return false;
}

void CAVPlayer::SetPanormaicType(PLAY_MODE type)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetPanormaicType(type);
}

void CAVPlayer::SetPanoramicScale(float factor)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetPanoramicScale(factor);
}

void CAVPlayer::SetPanoramicRotate(float x, float y)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetPanoramicRotate(x, y);
}

void CAVPlayer::SetPanoramicScroll(float latitude, float longitude)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetPanoramicScroll(latitude, longitude);
}

void CAVPlayer::SetWindowSize(int nWidth, int nHeight)
{
	if (m_pPlayerCore)
		m_pPlayerCore->SetWindowSize(nWidth, nHeight);
}
