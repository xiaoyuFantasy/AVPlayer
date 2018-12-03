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
	// 打开一个媒体文件.
	bool Open(PLAYER_OPTS &opts, bool bSync = false);

	// 开始播放视频.
	bool Play();

	// 暂停播放.
	void Pause();

	// 继续播放.
	void Resume();

	// 停止播放.
	void Stop();

	void SetRenderMode(AV_RENDER_MODE mode);

	// 播放状态.
	PLAY_STATUS Status();

	// 等待播放直到完成.
	bool WaitForCompletion();

	// 关闭媒体.
	void Close();

	// seek到某个时间播放, 按视频时长的百分比.
	void SeekTo(double fact);

	// 设置声音音量大小.
	void Volume(int nVolume);

	// 静音设置.
	void Mute(bool s);

	// 全屏切换.
	void FullScreen(bool bfull);

	// 返回当前播放时间.
	double CurrentPlayTime();

	// 当前播放视频的时长, 单位秒.
	double Duration();

	// 当前播放视频的宽高, 单位像素.
	void VideoSize(int &width, int &height);

	// 当前缓冲进度, 单位百分比.
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

