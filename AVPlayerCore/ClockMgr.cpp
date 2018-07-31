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
	CLock lock(m_mutexAudio);
	m_dAudioClock = dclock;
}

double CClockMgr::GetAudioClock()
{
	CLock lock(m_mutexAudio);
	return m_dAudioClock;
}
