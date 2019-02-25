#pragma once

#include "Core.h"
#include "ThreadingAPI.h"

class THREADING_API IThreading
{
public:
	
	static owner<IThreading*> create();
};