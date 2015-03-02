#include "rde/rde_string.h"

#include <stdio.h>
#include <float.h>
#include <stdarg.h>

namespace rde
{
	rde::string format(const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		int size = _vscprintf(fmt, vl) + 1;
		rde::string str;
		str.resize(size);
		vsnprintf_s((char*) str.c_str(), size, _TRUNCATE, fmt, vl);
		va_end(vl);

		return str;
	}

	rde::string to_string(int i)
	{
		char buf[11];
		sprintf_s(buf, "%i", i);
		return rde::string(buf);
	}
	rde::string to_string(unsigned int u)
	{
		char buf[11];
		sprintf_s(buf, "%u", u);
		return rde::string(buf);
	}
	rde::string to_string(float f)
	{
		char buf[40];
		sprintf_s(buf, "%f", f);
		return rde::string(buf);
	}
	rde::string to_string(uint64 i)
	{
		char buf[40];
		sprintf_s(buf, "%i64u", i);
		return rde::string(buf);
	}
}