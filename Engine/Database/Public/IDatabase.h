#pragma once

#include "Core.h"

class IDatabase
{
public:

	virtual owner<IDatabase*> create() = 0;
};