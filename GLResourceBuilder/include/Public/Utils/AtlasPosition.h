#pragma once

#include "BuilderCore.h"

struct AtlasPosition
{
	AtlasPosition() {}
	AtlasPosition(uint xPos, uint yPos, uint width, uint height, int atlasIdx) 
		: xPos(xPos), yPos(yPos), width(width), height(height), atlasIdx(atlasIdx) 
	{}
	uint xPos     = 0;
	uint yPos     = 0;
	uint width    = 0;
	uint height   = 0;
	int atlasIdx  = -1;

	bool isValid() { return width != 0 && height != 0 && atlasIdx != -1; }
};