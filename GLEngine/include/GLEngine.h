#pragma once

#include "Core.h"
#include <functional>

class Input;
class Graphics;
class Editor;

class GLEngine
{
public:

	static void initialize();
	static void initializeRenderThread(std::function<void()> func);
	static void shutdown();
	static void doMainThreadTick();
	static void sleep(uint timeMs);
	static uint getTimeMs();

private:

	GLEngine() {}
	~GLEngine() {}

	static int graphicsThread(void* ptr);

public:

	static Input* input;
	static Graphics* graphics;
	static Editor* editor;

private:

	static bool s_graphicsThreadExited;
};