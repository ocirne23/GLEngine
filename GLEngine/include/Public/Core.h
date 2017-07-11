#pragma once

#include "gsl/gsl.h"

#undef NULL
#define NULL nullptr
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

template <typename T>
void SAFE_DELETE(owner<T*>& owner)
{
	if (owner)
	{
		delete owner;
		owner = NULL;
	}
}

template <typename T>
void SAFE_DELETE_ARRAY(owner<T*>& owner)
{
	if (owner)
	{
		delete[] owner;
		owner = NULL;
	}
}