#pragma once

#if defined(DLL_BUILD)
#if defined(DATABASE_PROJECT)
#define DATABASE_API __declspec(dllexport)
#else
#define DATABASE_API __declspec(dllimport)
#endif
#else
#define DATABASE_API
#endif