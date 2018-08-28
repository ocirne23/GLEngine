#include "App.h"

#include "Graphics/Graphics.h"
#include "Graphics/IWindow.h"

#include "EASTL/string.h"

#include <iostream>
#include <stdio.h>

int main()
{
	/*
	char currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	eastl::string path(currentDir);

	SetDllDirectoryA("../../Engine/bin/x64/DebugDLL");
	*/
	App app;
	owner<IWindow*> window = app.graphics.createWindow("da", 200, 200, 0, 0, EWindowMode::WINDOWED);
	std::cin.get();
	app.graphics.destroyWindow(window);
	return 0;
}