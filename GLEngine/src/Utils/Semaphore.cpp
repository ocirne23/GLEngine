#include "Utils/Semaphore.h"

#include <SDL/SDL_thread.h>

Semaphore::Semaphore(uint a_numPermits) : m_semaphore(SDL_CreateSemaphore(a_numPermits))
{}

Semaphore::~Semaphore()
{
    SDL_DestroySemaphore(m_semaphore);
}

void Semaphore::acquire()
{
    SDL_SemWait(m_semaphore);
}

void Semaphore::release()
{
    SDL_SemPost(m_semaphore);
}