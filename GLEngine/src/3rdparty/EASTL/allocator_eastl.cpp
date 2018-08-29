/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>


///////////////////////////////////////////////////////////////////////////////
// ReadMe
//
// This file implements the default application allocator. 
// You can replace this allocator.cpp file with a different one,
// you can define EASTL_USER_DEFINED_ALLOCATOR below to ignore this file,
// or you can modify the EASTL config.h file to redefine how allocators work.
///////////////////////////////////////////////////////////////////////////////
//#define EASTL_USER_DEFINED_ALLOCATOR

#include <stdio.h>

// EASTL expects us to define these, see allocator.h line 194
void* operator new[](size_t size, const char* pName, int flags,
	unsigned debugFlags, const char* file, int line)
{
	return malloc(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
	const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	// this allocator doesn't support alignment
	EASTL_ASSERT(alignment <= 8);
	return malloc(size);
}

// EASTL also wants us to define this (see string.h line 197)
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}


#ifndef EASTL_USER_DEFINED_ALLOCATOR // If the user hasn't declared that he has defined an allocator implementation elsewhere...

	namespace eastl
	{

		/// gDefaultAllocator
		/// Default global allocator instance. 
		EASTL_API allocator   gDefaultAllocator;
		EASTL_API allocator* gpDefaultAllocator = &gDefaultAllocator;

		EASTL_API allocator* GetDefaultAllocator()
		{
			return gpDefaultAllocator;
		}

		EASTL_API allocator* SetDefaultAllocator(allocator* pAllocator)
		{
			allocator* const pPrevAllocator = gpDefaultAllocator;
			gpDefaultAllocator = pAllocator;
			return pPrevAllocator;
		}

	} // namespace eastl


#endif // EASTL_USER_DEFINED_ALLOCATOR











