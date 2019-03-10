#include "Core.h"

int CORE_API assert_handler(const char* str, const char* function, const char* file, int line)
{
	printf("assertion failed: (%s), %s : file %s L %d.\n", str, function, file, line);
	__debugbreak();
	return 0;
}