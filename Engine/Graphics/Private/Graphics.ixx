#pragma once

#include "Core.h"

export module Graphics.Graphics;

import Graphics.IWindow;
import Graphics.Window;
import Graphics.IGraphics;

export class Graphics final : public IGraphics
{
public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) override
	{
		return scast<IWindow*>(new Window(name, width, height, xPos, yPos, mode));
	}

	virtual void destroyWindow(owner<IWindow*>& window) override
	{
		delete scast<Window*>(window);
		window = nullptr;
	}

private:

	virtual ~Graphics() override {}
};

owner<IGraphics*> IGraphics::createGraphics()
{
	return new Graphics();
}

void IGraphics::destroyGraphics(owner<IGraphics*>& graphics)
{
	delete graphics;
	graphics = nullptr;
}