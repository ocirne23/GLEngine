#pragma once

#include "Core.h"
#include "ThreadingAPI.h"

class IThreading;

class THREADING_API ThreadingModule
{
public:

	static owner<IThreading*> createThreading();
	static void destroyThreading(owner<IThreading*> threading);

private:

	ThreadingModule() {}
	~ThreadingModule() {}
};