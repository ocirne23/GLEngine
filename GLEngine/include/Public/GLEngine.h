#pragma once

#include "Core.h"
#include <functional>

class Input;
class Graphics;
struct SDL_Thread;

class GLEngine
{
public:

	static void initialize();
	static void createRenderThread(std::function<void()> func);

	static void doMainThreadTick();
	static void doRenderThreadTick();

	static void sleep(uint timeMs);
	static uint getTimeMs();

	static void shutdown();
	static bool isShutdown() { return s_shutdown; }

private:

	GLEngine() {}
	~GLEngine() {}

	static int renderThread(void* ptr);
	static void dispose();

public:

	static Input* input;
	static Graphics* graphics;

private:

	static bool s_shutdown;
	static SDL_Thread* s_renderThread;
};