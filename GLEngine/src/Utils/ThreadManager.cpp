#include "Utils/ThreadManager.h"

#include <assert.h>
#include <SDL/SDL.h>
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()

// Because you cannot cast a std::function<void()> to a void* and back...
struct ThreadFuncWrapper
{
	ThreadFuncWrapper(const char* a_threadName, std::function<void()> a_func) : threadName(a_threadName), func(a_func) {}
	std::function<void()> func;
	std::string threadName;
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

int ThreadManager::threadFunc(owner<void*> a_ptr)
{
#pragma pack(push,8)  
	typedef struct tagTHREADNAME_INFO
	{
		tagTHREADNAME_INFO(const std::string& a_threadName): szName(a_threadName.c_str()) {}
		DWORD dwType = 0x1000; // Must be 0x1000.  
		LPCSTR szName;         // Pointer to name (in user addr space).  
		DWORD dwThreadID = -1; // Thread ID (-1=caller thread).  
		DWORD dwFlags = 0;     // Reserved for future use, must be zero.  
	} THREADNAME_INFO;
#pragma pack(pop)  
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
	ThreadFuncWrapper* func = scast<ThreadFuncWrapper*>(a_ptr);
	THREADNAME_INFO info(func->threadName);
	const DWORD MS_VC_EXCEPTION = 0x406D1388;
	__try { RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info); }
	__except (EXCEPTION_EXECUTE_HANDLER) { }
#pragma warning(pop)  
	func->func();
	delete a_ptr;
	return 0;
}

void ThreadManager::createThread(const char* a_threadName, std::function<void()> a_func)
{
	SDL_Thread* thread = SDL_CreateThread(&ThreadManager::threadFunc, a_threadName, new ThreadFuncWrapper(a_threadName, a_func));
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
