#include "stdafx.h"
#include "ClockMgr.h"
#include "Lock.h"

CClockMgr::CClockMgr()
{
}


CClockMgr::~CClockMgr()
{
}

void CClockMgr::SetSyncType(SYNC_TYPE type)
{
	m_syncType = type;
}

SYNC_TYPE CClockMgr::GetSyncType() const
{
	return m_syncType;
}

void CClockMgr::SetAudioStartTime(int64_t startTime)
{
	m_timeAudioStart = startTime;
}

int64_t CClockMgr::GetAudioStartTime() const
{
	return m_timeAudioStart;
}

void CClockMgr::SetVideoStartTime(int64_t startTime)
{
	m_timeVideoStart = startTime;
}

int64_t CClockMgr::GetVideoStartTime() const
{
	return m_timeVideoStart;
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

void CClockMgr::SetVideoClock(double dclock)
{
}

double CClockMgr::GetVideoClock()
{
	return 0.0;
}

double CClockMgr::GetExternalClock()
{
	int64_t t = av_gettime_relative() / 1000000.0;
	return 0.0;
}

double CClockMgr::GetMasterClock()
{
	double clock = 0.0;
	switch (m_syncType)
	{
	case AV_SYNC_AUDIO_MASTER:
		clock = GetAudioClock();
		break;
	case AV_SYNC_VIDEO_MASTER:
		clock = GetVideoClock();
		break;
	case AV_SYNC_EXTERNAL_CLOCK:
		clock = GetExternalClock();
		break;
	default:
		break;
	}
	return clock;
}
