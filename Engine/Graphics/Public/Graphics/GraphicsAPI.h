#pragma once

#if defined(DLL_BUILD)
#if defined(GRAPHICS_PROJECT)
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif
#else
#define GRAPHICS_API
#endif