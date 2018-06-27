#pragma once
#include "AVPlayerCore.h"

class CAVPlayer
{
public:
	CAVPlayer();
	virtual ~CAVPlayer();

	void Play(PLAYER_OPTS opts);
	void Stop();
	bool IsPlaying() const;
	void Pause(bool bPause = true);
	bool IsPaused() const;
	void Seek(int nPos);

	//audio
	void SetVolume(int nVolume);
	int GetVolume() const;
	void SetMuted(bool bMuted);
	bool IsMuted() const;
	//video
	void SetPanormaicType(PLAY_MODE type);
	void SetPanoramicScale(float factor);
	void SetPanoramicRotate(float x, float y);
	void SetPanoramicScroll(float latitude, float longitude);
	void SetWindowSize(int nWidth, int nHeight);

private:
	CAVPlayerCore*	m_pPlayerCore = nullptr;
	std::atomic_bool	m_bQuit = false;
};

