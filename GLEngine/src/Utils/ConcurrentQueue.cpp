#include "Utils/ConcurrentQueue.h"

#include <SDL/SDL_thread.h>

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue() : m_semaphore(SDL_CreateSemaphore(1))
{
}

template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
	SDL_DestroySemaphore(m_semaphore);
}

template <typename T>
void ConcurrentQueue<T>::push_back(const T& t)
{
	SDL_SemWait(m_semaphore);
	m_queue.push_back(t);
	SDL_SemPost(m_semaphore);
}

template <typename T>
T ConcurrentQueue<T>::pop_front()
{
	SDL_SemWait(m_semaphore);

	bool empty = m_queue.empty();
	T val = empty ? {0} : m_queue.front();
	if (!empty)
		m_queue.pop_front();
	SDL_SemPost(m_semaphore);
	return val;
}