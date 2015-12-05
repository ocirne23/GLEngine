#pragma once

#include "Core.h"

struct AtlasPosition
{
	AtlasPosition() {}
	AtlasPosition(uint xPos, uint yPos, uint width, uint height) 
		: xPos(xPos), yPos(yPos), width(width), height(height)
	{}
	uint xPos   = 0;
	uint yPos   = 0;
	uint width  = 0;
	uint height = 0;

	bool isValid() { return width != 0 && height != 0; }
};