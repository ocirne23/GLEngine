#pragma once

#if defined(DLL_BUILD)
#if defined(THREADING_PROJECT)
#define THREADING_API __declspec(dllexport)
#else
#define THREADING_API __declspec(dllimport)
#endif
#else
#define THREADING_API
#endif