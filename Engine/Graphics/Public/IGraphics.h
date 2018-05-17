#pragma once

#include "Core.h"

class IGraphics
{
public:

	virtual owner<IGraphics*> create() = 0;
};