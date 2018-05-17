#pragma once

#include "Core.h"

class IThreading
{
public:

	virtual owner<IThreading*> create() = 0;
};