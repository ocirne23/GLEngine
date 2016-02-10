#include "Core.h"

#include <stdarg.h>
#include <stdio.h>

void print(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	vprintf(a_format, args);
	// SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, a_format, args);
	va_end(args);
}