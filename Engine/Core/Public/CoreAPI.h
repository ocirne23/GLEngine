#pragma once

#if defined(DLL_BUILD)
#if defined(CORE_PROJECT)
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif