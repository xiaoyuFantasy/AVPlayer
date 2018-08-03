#pragma once
class CClockMgr
{
public:
	CClockMgr();
	virtual ~CClockMgr();
	int64_t GetAudioStartTime() const;
	void SetAudioClock(double dclock);
	double GetAudioClock();

private:
	double	m_dAudioClock = 0.0;
	std::mutex m_mutexAudio;
	std::atomic_int64_t	m_timeAudioStart = 0;
	std::atomic_int64_t m_timeVideoStart = 0;
};

