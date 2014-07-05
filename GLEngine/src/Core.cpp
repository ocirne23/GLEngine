#include "Core.h"

#include <SDL/SDL_log.h>

void print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, args);
	va_end(args);
}