#pragma once

enum SYNC_TYPE
{
	AV_SYNC_AUDIO_MASTER, /* Ĭ��ѡ��. */
	AV_SYNC_VIDEO_MASTER, /* ͬ������Ƶʱ���. */
	AV_SYNC_EXTERNAL_CLOCK, /* ͬ�����ⲿʱ��. */
};

class CClockMgr
{
public:
	CClockMgr();
	virtual ~CClockMgr();
	void SetSyncType(SYNC_TYPE type);
	SYNC_TYPE GetSyncType() const;
	void SetAudioStartTime(int64_t startTime);
	int64_t GetAudioStartTime() const;
	void SetVideoStartTime(int64_t startTime);
	int64_t GetVideoStartTime() const;
	void SetAudioClock(double dclock);
	double GetAudioClock();
	void SetVideoClock(double dclock);
	double GetVideoClock();
	double GetExternalClock();
	double GetMasterClock();

private:
	double	m_dAudioClock = 0.0;
	std::mutex m_mutexAudio;
	double	m_dVideoClock = 0.0;
	std::mutex m_mutexVideo;
	std::atomic_int64_t	m_timeAudioStart = 0;
	std::atomic_int64_t m_timeVideoStart = 0;
	SYNC_TYPE	m_syncType = AV_SYNC_AUDIO_MASTER;
};

