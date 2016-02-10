#pragma once

#undef NULL
#define NULL nullptr

#define SAFE_DELETE(X) if (X) {delete X; X = NULL; }
#define SAFE_DELETE_ARRAY(X) if (X) { delete[] X; X = NULL; }
#define BEGIN_NAMESPACE(X) namespace X {
#define END_NAMESPACE(X) }
#define BEGIN_UNNAMED_NAMESPACE() namespace {
#define END_UNNAMED_NAMESPACE() }
#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

#define scast static_cast
#define ccast const_cast
#define dcast dynamic_cast
#define rcast reinterpret_cast

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef long long int64;
typedef unsigned long long uint64;

void print(const char* format, ...);