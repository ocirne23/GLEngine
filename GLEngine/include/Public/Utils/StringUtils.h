#pragma once

#include "EASTL/string.h"
#include <stdio.h>
#include <float.h>
#include <stdarg.h>

class StringUtils
{
public:
	static eastl::string format(const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		int size = _vscprintf(fmt, vl) + 1;
		eastl::string str;
		str.resize(size);
		vsnprintf_s((char*) str.c_str(), size, _TRUNCATE, fmt, vl);
		va_end(vl);
		return str;
	}

	static eastl::string to_string(int i)
	{
		char buf[11];
		sprintf_s(buf, "%i", i);
		return eastl::string(buf);
	}
	static eastl::string to_string(unsigned int u)
	{
		char buf[11];
		sprintf_s(buf, "%u", u);
		return eastl::string(buf);
	}
	static eastl::string to_string(float f)
	{
		char buf[40];
		sprintf_s(buf, "%f", f);
		return eastl::string(buf);
	}
	static eastl::string to_string(uint64 i)
	{
		char buf[40];
		sprintf_s(buf, "%i64u", i);
		return eastl::string(buf);
	}
};