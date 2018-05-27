#pragma once

#include "Core.h"

class IInput
{
public:

	virtual owner<IInput*> create() = 0;
};