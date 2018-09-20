#include "App.h"

#include "Core/Utils.h"
#include "Graphics/GraphicsModule.h"
#include "Graphics/IGraphics.h"
#include "Graphics/IWindow.h"
#include "Graphics/IContext.h"

#include <iostream>
#include <stdio.h>

int main()
{
	auto graphics = GraphicsModule::createGraphics();
	owner<IWindow*> window = graphics->createWindow("da", 640, 480, 700, 250, EWindowMode::WINDOWED);
	owner<IContext*> context = window->createContext(EContextType::OPENGL);
	window->swapBuffer();
	std::cin.get();
	window->destroyContext(context);
	graphics->destroyWindow(window);
	GraphicsModule::destroyGraphics(graphics);
	return 0;
}

// EASTL new defines
#if defined(LIB_BUILD)
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return new char[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return operator new[](size, pName, flags, debugFlags, file, line);
}
#endif