#pragma once

#include "gsl/gsl.h"
using namespace gsl;

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

using uint = unsigned int;
using ushort = unsigned short;
using int64 = long long;
using uint64 = unsigned long long;

template <typename T, std::size_t N>
constexpr std::size_t ARRAY_SIZE(T const (&)[N]) noexcept
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