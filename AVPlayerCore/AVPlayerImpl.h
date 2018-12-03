#pragma once
#include "AVPlayerDefine.h"
#include "AudioPlayer.h"
#include "VideoPlayer.h"

class CAVPlayerImpl
{
public:
	CAVPlayerImpl();
	virtual ~CAVPlayerImpl();

public:
	// ��һ��ý���ļ�.
	bool Open(PLAYER_OPTS &opts, bool bSync = false);

	// ��ʼ������Ƶ.
	bool Play();

	// ��ͣ����.
	void Pause();

	// ��������.
	void Resume();

	// ֹͣ����.
	void Stop();

	void SetRenderMode(AV_RENDER_MODE mode);

	// ����״̬.
	PLAY_STATUS Status();

	// �ȴ�����ֱ�����.
	bool WaitForCompletion();

	// �ر�ý��.
	void Close();

	// seek��ĳ��ʱ�䲥��, ����Ƶʱ���İٷֱ�.
	void SeekTo(double fact);

	// ��������������С.
	void Volume(int nVolume);

	// ��������.
	void Mute(bool s);

	// ȫ���л�.
	void FullScreen(bool bfull);

	// ���ص�ǰ����ʱ��.
	double CurrentPlayTime();

	// ��ǰ������Ƶ��ʱ��, ��λ��.
	double Duration();

	// ��ǰ������Ƶ�Ŀ��, ��λ����.
	void VideoSize(int &width, int &height);

	// ��ǰ�������, ��λ�ٷֱ�.
	double Buffering();

	//video scale
	void SetScale(float factor);

	//video scroll
	void SetRotate(double xoffset, double yoffset);

protected:
	static int interrupt_callback(void * lparam);
	void CallbackEvent(PLAYER_EVENT e, LPVOID lpData);

private:
	PLAYER_OPTS		m_opts;
	std::mutex		m_mutexEvent;
	AVFormatContext*	m_pFormatCtx = nullptr;
	int64_t				m_duration = 0;
	std::atomic<PLAY_STATUS> m_statusPlayer = PLAY_STATUS::NoneStatus;
	std::thread			m_threadOpen;
	std::thread			m_threadPlay;
	std::atomic_bool	m_bOpened = false;
	std::atomic_bool	m_bOpening = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_bool	m_bStop = false;
	std::atomic_bool	m_bPause = false;
	std::mutex			m_mxPauseWait;
	std::condition_variable		m_cvPause;
	std::atomic_bool	m_bSeek = false;
	std::atomic<double>	m_nPos = 0.0;
	
	int					m_nVideoIndex = -1;
	int					m_nAudioIndex = -1;
	bool				m_bAudioOpen = false;
	bool				m_bVideoOpen = false;
	CVideoPlayer		m_videoPlayer;
	CAudioPlayer		m_audioPlayer;	
	std::shared_ptr<IRender> m_pRender = nullptr;
	std::shared_ptr<ISound> m_pSound = nullptr;
	CClockMgr			m_clockMgr;
};

