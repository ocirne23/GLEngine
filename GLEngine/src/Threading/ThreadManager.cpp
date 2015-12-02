#include "Threading/ThreadManager.h"

#include "Core.h"

#include <assert.h>
#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

// Because you cannot cast a std::function<void()> to a void* and back...
struct FuncWrapper
{
	FuncWrapper(std::function<void()> a_func) : func(a_func) {}
	std::function<void()> func;
};

END_UNNAMED_NAMESPACE()

ThreadManager::ThreadManager()
{
	m_mainThreadID = SDL_ThreadID();
}

ThreadManager::~ThreadManager()
{
	assert(m_threads.empty());
}

int ThreadManager::threadFunc(void* a_ptr)
{
	((FuncWrapper*) a_ptr)->func();
	delete a_ptr;
	return 0;
}

void ThreadManager::createThread(const char* a_threadName, std::function<void()> a_func)
{
	SDL_Thread* thread = SDL_CreateThread(&ThreadManager::threadFunc, a_threadName, new FuncWrapper(a_func));
	assert(thread);
	m_threads.push_back(thread);
}

void ThreadManager::waitForAllThreadShutdown()
{
	for (SDL_Thread* thread : m_threads)
	{
		int retval = 0;
		print("Waiting for thread: %s to finish\n", SDL_GetThreadName(thread));
		SDL_WaitThread(thread, &retval);
		assert(!retval);
	}
	m_threads.clear();
}
