#include "Threading/ThreadingModule.h"

#include "Threading.h"

owner<IThreading*> ThreadingModule::createThreading()
{
	return owner<IThreading*>(new Threading());
}

void ThreadingModule::destroyThreading(owner<IThreading*> threading)
{
	assert(threading);
	delete static_cast<Threading*>(threading);
}
