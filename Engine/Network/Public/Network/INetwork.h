#pragma once

#include "Core.h"

class INetwork
{
public:

	virtual owner<INetwork*> create() = 0;
};