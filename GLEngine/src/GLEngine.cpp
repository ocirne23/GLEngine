#include "GLEngine.h"

#include "Editor/Editor.h"
#include "Graphics/Graphics.h"
#include "Graphics/WindowFlags.h"
#include "Input/Input.h"
#include "Utils/WindowsPlatformData.h"

#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

static const char* const PROGRAM_NAME = "GLEngine";
static const uint INIT_WINDOW_WIDTH = 1024;
static const uint INIT_WINDOW_HEIGHT = 768;
static const uint INIT_WINDOW_XPOS = 25;// +1920;
static const uint INIT_WINDOW_YPOS = 40;

END_UNNAMED_NAMESPACE()

Input* GLEngine::input					= NULL;
Graphics* GLEngine::graphics			= NULL;
Editor* GLEngine::editor				= NULL;
bool GLEngine::s_graphicsThreadExited	= false;

void GLEngine::initializeRenderThread(std::function<void()> a_func)
{
	SDL_CreateThread(&GLEngine::graphicsThread, "RenderThread", &a_func);
}

int GLEngine::graphicsThread(void* a_ptr)
{
	std::function<void()> func = *((std::function<void()>*) a_ptr);
	graphics->initializeGLContext();
	func();
	s_graphicsThreadExited = true;
	return 0;
}

void GLEngine::initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		print("%s: %s\n", "Unable to initialize SDL", SDL_GetError());
		SDL_Quit();
		return;
	}

	graphics = new Graphics();
	input = new Input();
	graphics->initializeWindow(
		PROGRAM_NAME, 
		INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, 
		INIT_WINDOW_XPOS, INIT_WINDOW_YPOS, 
		(WINDOW_OPENGL | WINDOW_SHOWN | WINDOW_RESIZABLE));
#ifdef EDITOR
	editor = new Editor();
#endif
}

void GLEngine::doMainThreadTick()
{
	input->processEvents();
#ifdef EDITOR
	editor->updateUIPosition();
#endif
}

void GLEngine::sleep(uint a_timeMs)
{
	SDL_Delay(a_timeMs);
}

uint GLEngine::getTimeMs()
{
	return SDL_GetTicks();
}

void GLEngine::shutdown()
{
	while (!s_graphicsThreadExited)
		sleep(1);

	if (graphics->hasWindow())
		graphics->windowQuit();

	delete input;
	delete graphics;
#ifdef EDITOR
	editor->quit();
	delete editor;
#endif
	SDL_Quit();
}