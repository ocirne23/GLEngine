#pragma once

#include "Core.h"
#include "rde/rde_string.h"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

struct SDL_Window;

class WindowsPlatformData
{
public:

    static bool registerWindowsClass(const char* className);
    static void setWindowHandle(SDL_Window* window);
    static void setEditorTop();
    static void quitEditor();

public:

    static void* s_editorWindowHandle;
    static void* s_windowHandle;
    static rde::string s_className;
};