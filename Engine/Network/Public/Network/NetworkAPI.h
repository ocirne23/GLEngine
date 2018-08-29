#pragma once

#if defined(DLL_BUILD)
#if defined(NETWORK_PROJECT)
#define NETWORK_API __declspec(dllexport)
#else
#define NETWORK_API __declspec(dllimport)
#endif
#else
#define NETWORK_API
#endif