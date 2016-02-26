#pragma once

#include "Core.h"
#include "gsl/gsl.h"

#include <functional>

class Input;
class Graphics;
class ThreadManager;

enum class EWindowMode { WINDOWED, BORDERLESS, FULLSCREEN, NONE };

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
class GLEngine
{
public:
	/** Should be called prior to anything else related to the library */
	static void initialize(const char* windowName, uint width, uint height, EWindowMode windowMode);
	static void createThread(const char* threadName, std::function<void()> func);

	/** Setup a thread with a GL context to allow for rendering */
	static void createGLContext();
	/** Destroy the GL context for this thread */
	static void destroyGLContext();

	/** Should be called continuously on the main thread to receive input events from the OS */
	static void doMainThreadTick();
	/** Should be called on the users logic thread to trigger input listeners */
	static void doEngineTick();

	static void sleep(uint timeMs);
	static uint getTimeMs();

	/** Set the shutdown flag so all created threads that are using isShutdown() to loop can finish */
	static void shutdown()   { s_shutdown = true; }
	static bool isShutdown() { return s_shutdown; }

	/** Clean up all engine resources and destroy the window, waits for all created threads to finish */
	static void finish();

private:

	GLEngine() {}
	~GLEngine() {}

public:

	static owner<Input*> input;
	static owner<Graphics*> graphics;

private:

	static bool s_shutdown;
	static owner<ThreadManager*> s_threadManager;
};