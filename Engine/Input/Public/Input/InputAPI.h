#pragma once

#if defined(DLL_BUILD)
#if defined(INPUT_PROJECT)
#define INPUT_API __declspec(dllexport)
#else
#define INPUT_API __declspec(dllimport)
#endif
#else
#define INPUT_API
#endif