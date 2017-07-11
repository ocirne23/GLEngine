#pragma once

#include "EASTL/vector.h"

#include <functional>
#include <SDL/SDL.h>

struct SDL_Thread;

class ThreadManager
{
public:

	ThreadManager();
	~ThreadManager();

	void createThread(const char* threadName, std::function<void()> func);
	void waitForAllThreadShutdown();

private:

	static int threadFunc(void* ptr);

private:

	SDL_threadID m_mainThreadID;
	eastl::vector<SDL_Thread*> m_threads;
};
