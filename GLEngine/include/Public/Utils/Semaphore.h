#pragma once

#include "Core.h"

struct SDL_Semaphore;
typedef struct SDL_semaphore SDL_sem;

class Semaphore
{
public:

	Semaphore(uint numPermits = 1);
	~Semaphore();

	void acquire();
	void release();

private:

	SDL_sem* m_semaphore;
};