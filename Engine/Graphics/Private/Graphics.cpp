#include "Graphics.h"

#include "Window.h"

owner<IGraphics*> IGraphics::createGraphics()
{
	return new Graphics();
}

void IGraphics::destroyGraphics(owner<IGraphics*>& graphics)
{
	delete graphics;
	graphics = nullptr;
}

owner<IWindow*> Graphics::createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
{
	return new Window(name, width, height, xPos, yPos, mode);
}

void Graphics::destroyWindow(owner<IWindow*>& window)
{
	delete static_cast<Window*>(window);
	window = nullptr;
}
