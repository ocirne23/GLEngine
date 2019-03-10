#pragma once

#include "CoreAPI.h"

import std.core;

template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner = T;

#undef NULL
#define NULL nullptr
#define BEGIN_NAMESPACE(X) namespace X {
#define END_NAMESPACE(X) }
#define BEGIN_UNNAMED_NAMESPACE() namespace {
#define END_UNNAMED_NAMESPACE() }

#define scast static_cast
#define ccast const_cast
#define dcast dynamic_cast
#define rcast reinterpret_cast

template <typename CastType, typename SourceType>
constexpr CastType alias_cast(SourceType arg)
{
	static_assert(sizeof(CastType) == sizeof(SourceType));
	union
	{
		CastType type;
		SourceType source;
	} u;
	u.source = arg;
	return u.type;
}

#define acast alias_cast

using byte = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;
using int64 = long long;
using uint64 = unsigned long long;

void print(const char* a_format, ...);

#define assert(x) ((void)(!(x) && assert_handler(#x, __FUNCTION__, __FILE__, __LINE__)))

int CORE_API assert_handler(const char* str, const char* function, const char* file, int line);

template <typename T, size_t N>
constexpr size_t ARRAY_SIZE(T const (&)[N])
{
	return N;
}

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
