#include "Graphics/Graphics.h"

#include "Window.h"

owner<IWindow*> Graphics::createWindow(const char* a_name, uint a_width, uint a_height, uint a_xPos, uint a_yPos, const EWindowMode& a_mode)
{
	return new Window(a_name, a_width, a_height, a_xPos, a_yPos, a_mode);
}

void Graphics::destroyWindow(owner<IWindow*> window)
{
	assert(window);
	delete window;
}
