#pragma once

#if defined(DATABASE_PROJECT)
#define DATABASE_API __declspec(dllexport)
#else
#define DATABASE_API __declspec(dllimport)
#endif