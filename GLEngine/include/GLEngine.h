#pragma once

#ifdef ANDROID
#define MAIN SDL_main
extern "C" int SDL_main(int argc, char *argv[]);
#else
#define MAIN main
#endif

#include "Core.h"

class Input;
class Graphics;

class GLEngine
{
public:
	static void initialize();
	static void shutdown();

	static void doEngineTick();

	static void sleep(unsigned int timeMs);
	static uint getTimeMs();

private:
	GLEngine() {}
	~GLEngine() {}

public:
	static Input* input;
	static Graphics* graphics;
};