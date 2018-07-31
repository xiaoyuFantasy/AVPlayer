#pragma once
class CClockMgr
{
public:
	CClockMgr();
	virtual ~CClockMgr();

	void SetAudioClock(double dclock);
	double GetAudioClock();

private:
	double	m_dAudioClock = 0.0;
	std::mutex m_mutexAudio;
	
};

