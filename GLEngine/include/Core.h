#pragma once

#define NULL 0

#ifdef ANDROID
#define OVERRIDE
#else
#define OVERRIDE override
#endif //ANDROID

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef long long int64;
typedef unsigned long long uint64;

void print(const char* format, ...);