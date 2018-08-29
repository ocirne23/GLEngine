#include "App.h"

#include "Core/Utils.h"
#include "Graphics/Graphics.h"
#include "Graphics/IWindow.h"

#include "EASTL/string.h"

#include <iostream>
#include <stdio.h>

int main()
{
	App app;
	owner<IWindow*> window = app.graphics.createWindow("da", 200, 200, 0, 0, EWindowMode::WINDOWED);
	std::cin.get();
	app.graphics.destroyWindow(window);
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