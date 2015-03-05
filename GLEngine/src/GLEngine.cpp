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

Input* GLEngine::input = NULL;
Graphics* GLEngine::graphics = NULL;
Editor* GLEngine::editor = NULL;

void GLEngine::initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		print("%s: %s\n", "Unable to initialize SDL", SDL_GetError());
		SDL_Quit();
	}

	graphics = new Graphics();
	input = new Input();
	graphics->initialize(
		PROGRAM_NAME, 
		INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, 
		INIT_WINDOW_XPOS, INIT_WINDOW_YPOS, 
		(WINDOW_OPENGL | WINDOW_SHOWN | WINDOW_RESIZABLE));
#ifdef EDITOR
	editor = new Editor();
#endif
}

void GLEngine::doEngineTick()
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