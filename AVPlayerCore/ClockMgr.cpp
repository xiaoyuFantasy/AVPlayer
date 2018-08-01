#include "stdafx.h"
#include "ClockMgr.h"
#include "Lock.h"

CClockMgr::CClockMgr()
{
}


CClockMgr::~CClockMgr()
{
}

void CClockMgr::SetAudioClock(double dclock)
{
	CAutoLock lock(m_mutexAudio);
	m_dAudioClock = dclock;
}

double CClockMgr::GetAudioClock()
{
	CAutoLock lock(m_mutexAudio);
	return m_dAudioClock;
}
