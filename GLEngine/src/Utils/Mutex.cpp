#include "Utils/Mutex.h"

#include <SDL/SDL_mutex.h>

Mutex::Mutex() 
	: m_mutex(SDL_CreateMutex())
{
}

Mutex::~Mutex()
{
	SDL_DestroyMutex(m_mutex);
}

void Mutex::lock()
{
	SDL_LockMutex(m_mutex);
}

void Mutex::unlock()
{
	SDL_UnlockMutex(m_mutex);
}