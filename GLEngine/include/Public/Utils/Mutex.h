#pragma once

struct SDL_mutex;
typedef struct SDL_mutex SDL_mutex;

class Mutex
{
public:

	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:

	SDL_mutex* m_mutex;
};