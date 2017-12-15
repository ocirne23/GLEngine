#include "Utils/ScopeLock.h"

#include "Utils/Mutex.h"

ScopeLock::ScopeLock(Mutex& a_mutex) : m_mutex(a_mutex)
{
	m_mutex.lock();
}

ScopeLock::~ScopeLock()
{
	m_mutex.unlock();
}