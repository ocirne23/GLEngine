#pragma once

#include "Core.h"

class Input;
class Graphics;
class Editor;

class GLEngine
{
public:

	static void initialize();
	static void shutdown();
	static void doEngineTick();
	static void sleep(uint timeMs);
	static uint getTimeMs();

private:

	GLEngine() {}
	~GLEngine() {}

public:

	static Input* input;
	static Graphics* graphics;
	static Editor* editor;
};