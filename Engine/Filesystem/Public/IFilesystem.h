#pragma once

#include "Core.h"

class IFilesystem
{
public:

	virtual owner<IFilesystem*> create() = 0;
};