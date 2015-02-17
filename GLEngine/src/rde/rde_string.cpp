#include "rde/rde_string.h"

#include <stdio.h>
#include <float.h>

namespace rde
{
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
}