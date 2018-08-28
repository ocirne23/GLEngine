#pragma once

#if defined(_WINDLL)
#if defined(FILESYSTEM_PROJECT)
#define FILESYSTEM_API __declspec(dllexport)
#else
#define FILESYSTEM_API __declspec(dllimport)
#endif
#else
#define FILESYSTEM_API
#endif