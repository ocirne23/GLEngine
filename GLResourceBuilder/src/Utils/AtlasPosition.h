#pragma once

#include "BuilderCore.h"

struct AtlasPosition
{
	AtlasPosition() {}
	uint xPos     = 0;
	uint yPos     = 0;
	uint width    = 0;
	uint height   = 0;
	int atlasIdx  = -1;
};