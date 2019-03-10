#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

export module Graphics.IGraphics;
import Graphics.IWindow;

export class GRAPHICS_API IGraphics
{
public:

	static owner<IGraphics*> createGraphics();
	static void destroyGraphics(owner<IGraphics*>& graphics);

public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) = 0;
	virtual void            destroyWindow(owner<IWindow*>& window) = 0;

protected:

	virtual ~IGraphics() {}
};