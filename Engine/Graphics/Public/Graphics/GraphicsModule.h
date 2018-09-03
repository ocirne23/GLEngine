#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

class IGraphics;

class GRAPHICS_API GraphicsModule
{
public:

	static owner<IGraphics*> createGraphics();
	static void destroyGraphics(owner<IGraphics*> graphics);

private:

	GraphicsModule() {}
	~GraphicsModule() {}
};