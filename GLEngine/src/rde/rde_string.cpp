#include "rde\rde_string.h"

#include <stdio.h>
#include <float.h>

namespace rde
{
	rde::string to_string(int i)
	{
		char buf[11];
#ifdef ANDROID
		sprintf(buf, "%i", i);
#else
		sprintf_s(buf, "%i", i);
#endif
		return rde::string(buf);
	}
	rde::string to_string(unsigned int u)
	{
		char buf[11];
#ifdef ANDROID
		sprintf(buf, "%u", u);
#else
		sprintf_s(buf, "%u", u);
#endif
		return rde::string(buf);
	}
	rde::string to_string(float f)
	{
		char buf[40];
#ifdef ANDROID
		sprintf(buf, "%f", f);
#else
		sprintf_s(buf, "%f", f);
#endif

		return rde::string(buf);
	}
}