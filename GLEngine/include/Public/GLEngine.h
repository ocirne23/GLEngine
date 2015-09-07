#pragma once

#include "Core.h"
#include <functional>

/**********************
Main class providing basic functions and static access to input and graphics

*** Usage examples ***
Setup and main loop:

#include "GLEngine.h"
int main()
{
	GLEngine::initialize();
	GLEngine::createThread("RenderThread", [&]()
	{
		GLEngine::createGLContext();
		while (!GLEngine::isShutdown())
		{
			GLEngine::doRenderThreadTick();
			// render stuff
		}
		GLEngine::destroyGLContext();
	});
	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick()
	GLEngine::finish();
}
**********************/

class Input;
class Graphics;
class ThreadManager;

class GLEngine
{
public:

	static void initialize();
	static void createThread(const char* threadName, std::function<void()> func);
	static void createGLContext();
	static void destroyGLContext();

	static void doMainThreadTick();
	static void doEngineTick();

	static void sleep(uint timeMs);
	static uint getTimeMs();

	static void shutdown();
	static bool isShutdown() { return s_shutdown; }
	static void finish();

private:

	GLEngine() {}
	~GLEngine() {}

public:

	static Input* input;
	static Graphics* graphics;

private:

	static bool s_shutdown;
	static ThreadManager* s_threadManager;
};