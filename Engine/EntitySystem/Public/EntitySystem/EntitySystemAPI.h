#pragma once

#if defined(DLL_BUILD)
#if defined(ENTITYSYSTEM_PROJECT)
#define ENTITYSYSTEM_API __declspec(dllexport)
#else
#define ENTITYSYSTEM_API __declspec(dllimport)
#endif
#else
#define ENTITYSYSTEM_API
#endif