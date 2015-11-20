#pragma once

#include "Core.h"

class GLDrawCall
{
private:
	GLDrawCall() {}
	~GLDrawCall() {}
public:

	static void draw(uint numIndices, uint indiceBufferFormat);

private:
};