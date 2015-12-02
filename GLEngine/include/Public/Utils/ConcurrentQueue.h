#pragma once

#include "EASTL/list.h"
#include "Utils/Semaphore.h"

template <typename T>
class ConcurrentQueue
{
public:

	ConcurrentQueue(T emptyVal = {0}) : m_emptyVal(emptyVal), m_semaphore(1) {}

	void push_back(T& val)
	{
		m_semaphore.acquire();
		m_queue.push_back(val);
		m_semaphore.release();
	}

	T pop_front()
	{
		m_semaphore.acquire();
		bool empty = m_queue.empty();
		T val = empty ? m_emptyVal : m_queue.front();
		if (!empty)
			m_queue.pop_front();
		m_semaphore.release();
		return val;
	}

	void clear()
	{
		m_semaphore.acquire();
		m_queue.clear();
		m_semaphore.release();
	}

	void block() { m_semaphore.acquire(); }
	void release() { m_semaphore.release(); }
	eastl::list<T>& getBackingQueue() { return m_queue; }

private:

	Semaphore m_semaphore;
	T m_emptyVal;
	eastl::list<T> m_queue;
};