#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

class GRAPHICS_API IGraphics
{
public:

	static owner<IGraphics*> create();
};