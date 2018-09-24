#pragma once
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include "Lock.h"

template <class T>
class CQueue
{
public:
	CQueue(const size_t sizeMax);
	virtual ~CQueue();

	void	Init();
	size_t	Size();
	size_t	Count();
	void	Quit();
	void	Clear();

	bool Push(T &&data);
	bool Pop(T &data);

private:
	size_t			m_size;
	std::queue<T>	m_queue;
	std::mutex		m_mutexQueue;
	std::mutex		m_mutex;
	std::atomic_bool	m_quit = false;
	std::condition_variable	m_cvFull;
	std::condition_variable m_cvEmpty;
};

template<class T>
CQueue<T>::CQueue(const size_t sizeMax)
	:m_size(sizeMax)
{

}

template<class T>
CQueue<T>::~CQueue()
{
}

template<class T>
void CQueue<T>::Init()
{
	Quit();
	m_quit = false;
}

template<class T>
size_t CQueue<T>::Size()
{
	return m_size;
}

template<class T>
size_t CQueue<T>::Count()
{
	CAutoLock lock(m_mutexQueue);
	return m_queue.size();
}

template<class T>
void CQueue<T>::Quit()
{
	m_quit = true;
	m_cvEmpty.notify_all();
	m_cvFull.notify_all();
	Clear();
}

template<class T>
bool CQueue<T>::Push(T && data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (!m_quit)
	{
		if (Count() >= m_size)
			m_cvFull.wait(lock);
		{
			m_mutexQueue.lock();
			m_queue.push(std::move(data));
			m_mutexQueue.unlock();
		}
		m_cvEmpty.notify_one();
		return true;
	}

	return false;
}

template<class T>
bool CQueue<T>::Pop(T & data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (!m_quit)
	{
		if (Count() == 0)
			m_cvEmpty.wait(lock);
		{
			CAutoLock lock(m_mutexQueue);
			if (m_queue.size() > 0)
			{
				data = std::move(m_queue.front());
				m_queue.pop();
			}
			else
				return false;
		}
		m_cvFull.notify_one();
		return true;
	}
	return false;
}

template<class T>
void CQueue<T>::Clear()
{
	m_mutexQueue.lock();
	while (!m_queue.empty())
		m_queue.pop();
	m_mutexQueue.unlock();
}
