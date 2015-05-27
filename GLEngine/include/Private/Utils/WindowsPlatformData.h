#pragma once

#include "Core.h"
#include "3rdparty/rde/rde_string.h"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

struct SDL_Window;

class WindowsPlatformData
{
public:

	static bool registerWindowsClass(const char* className);
	static void setWindowHandle(SDL_Window* window);

public:

	static void* s_windowHandle;
	static rde::string s_className;
};