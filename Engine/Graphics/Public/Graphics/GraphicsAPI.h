#pragma once

#if defined(_WINDLL)
#if defined(GRAPHICS_PROJECT)
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif
#else
#define GRAPHICS_API
#endif