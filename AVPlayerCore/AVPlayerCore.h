#pragma once
#include "AVPlayerDefine.h"
#include "SoundDefine.h"
#include "SoundFactory.h"
#include "RenderDefine.h"
#include "RenderFactory.h"

#include "AudioPlayer.h"
#include "VideoPlayer.h"
#include "ClockMgr.h"


class CAVPlayerCore
{
public:
	CAVPlayerCore(PLAYER_OPTS &opts);
	virtual ~CAVPlayerCore();

public:
	const char * GetPath();
	bool Play();
	bool IsPlaying() const;
	void Pause(bool bPause = true);
	bool IsPaused() const;
	void Stop();
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

protected:
	static int interrupt_callback(void * lparam);

private:
	PLAYER_OPTS		m_opts;
	AVFormatContext*	m_pFormatCtx = nullptr;
	int64_t				m_duration = 0;
	std::atomic_bool	m_bQuit = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_bool	m_bPaused = false;
	std::atomic_bool	m_bSeek = false;
	std::atomic<double>	m_nPos = 0.0;
	int					m_nVideoIndex = -1;
	int					m_nAudioIndex = -1;
	bool				m_bAudioOpen = false;
	bool				m_bVideoOpen = false;
	CVideoPlayer*		m_pVideoPlayer = nullptr;
	CAudioPlayer*		m_pAudioPlayer = nullptr;
	IRender*			m_pRender = nullptr;
	ISound*				m_pSound = nullptr;
	CClockMgr			m_clockMgr;
};
