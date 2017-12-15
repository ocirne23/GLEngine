#pragma once

class Mutex;

class ScopeLock
{
public:

	ScopeLock(Mutex& mutex);
	~ScopeLock();

private:

	Mutex& m_mutex;
};