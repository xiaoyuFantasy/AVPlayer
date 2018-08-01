#pragma once
#include <mutex>

class CAutoLock
{
public:
	CAutoLock(std::mutex &mx)
		:m_mutex(mx)
	{
		m_mutex.lock();
	}
	virtual ~CAutoLock() 
	{
		m_mutex.unlock();
	}

private:
	std::mutex &m_mutex;
};

