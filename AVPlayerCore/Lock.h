#pragma once
#include <mutex>

class CLock
{
public:
	CLock(std::mutex &mx)
		:m_mutex(std::move(mx))
	{
		m_mutex.lock();
	}
	virtual ~CLock() 
	{
		m_mutex.unlock();
	}

private:
	std::mutex &&m_mutex;
};

