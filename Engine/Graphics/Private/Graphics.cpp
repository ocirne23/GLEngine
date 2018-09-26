#include "Graphics.h"

#include "Window.h"
#include "Camera.h"
#include "EASTL/vector.h"

Graphics::~Graphics()
{
#ifdef DEBUG_BUILD
	assert(md_numWindowsCreated == 0);
	assert(md_numCamerasCreated == 0);
#endif
}

owner<IWindow*> Graphics::createWindow(const char* a_name, uint a_width, uint a_height, uint a_xPos, uint a_yPos, const EWindowMode& a_mode)
{
#ifdef DEBUG_BUILD
	md_numWindowsCreated++;
#endif
	return new Window(a_name, a_width, a_height, a_xPos, a_yPos, a_mode);
}

void Graphics::destroyWindow(owner<IWindow*> window)
{
#ifdef DEBUG_BUILD
	md_numWindowsCreated--;
#endif
	assert(window);
	delete window;
}

owner<ICamera*> Graphics::createCamera()
{
#ifdef DEBUG_BUILD
	md_numCamerasCreated++;
#endif
	return owner<ICamera*>(new Camera());
}

void Graphics::destroyCamera(owner<ICamera*> camera)
{
#ifdef DEBUG_BUILD
	md_numCamerasCreated--;
#endif
	assert(camera);
	delete scast<ICamera*>(camera);
}
