#pragma once

#include "Core.h"

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

	static size_t getStackUsage();

private:

	static int threadFunc(owner<void*> ptr);

private:

	SDL_threadID m_mainThreadID;
	eastl::vector<SDL_Thread*> m_threads;
};
