#pragma once
#include "AVPlayerDefine.h"
#include "AudioPlayer.h"
#include "VideoPlayer.h"

class CPlayerImpl
{
public:
	CPlayerImpl();
	virtual ~CPlayerImpl();

public:
	// 打开一个媒体文件
	bool Open(PLAYER_OPTS &opts, bool bSync = true);

	// 开始播放视频.
	bool Play();

	// 暂停播放.
	void Pause();

	// 继续播放.
	void Resume();

	// 停止播放.
	bool Stop();

	// 等待播放直到完成.
	bool WaitForCompletion();

	// 关闭媒体
	bool Close();

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
	bool VideoSize(int &width, int &height);

	// 当前缓冲进度, 单位百分比.
	double Buffering();

protected:
	static int interrupt_callback(void * lparam);

private:
	PLAYER_OPTS		m_opts;
	AVFormatContext*	m_pFormatCtx = nullptr;
	int64_t				m_duration = 0;
	std::thread			m_threadPlay;
	std::atomic_bool	m_bClose = false;
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
	IRender*			m_pRender = nullptr;
	ISound*				m_pSound = nullptr;
	CClockMgr			m_clockMgr;
};

