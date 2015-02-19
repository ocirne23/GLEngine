#include "GLEngine.h"

#include "Graphics/Graphics.h"
#include "Graphics/WindowFlags.h"
#include "Input/Input.h"

#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

static const char* const PROGRAM_NAME = "GLEngine";
static const uint INIT_WINDOW_WIDTH = 1024;
static const uint INIT_WINDOW_HEIGHT = 768;
static const uint INIT_WINDOW_XPOS = 5;// +1920;
static const uint INIT_WINDOW_YPOS = 25;

END_UNNAMED_NAMESPACE()

Input* GLEngine::input = NULL;
Graphics* GLEngine::graphics = NULL;

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
}

void GLEngine::doEngineTick()
{
	SDL_Event event;

	while (graphics->getWindow() && SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				graphics->windowResize(event.window.data1, event.window.data2);
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			input->mouseMoved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
		case SDL_MOUSEBUTTONDOWN:
			input->mouseDown((MouseButton) event.button.button, event.button.x, event.button.y);
			break;
		case SDL_MOUSEBUTTONUP:
			input->mouseUp((MouseButton) event.button.button, event.button.x, event.button.y);
			break;
		case SDL_MOUSEWHEEL:
			input->mouseScrolled(event.wheel.y);
			break;
		case SDL_KEYDOWN:
			input->keyDown((Key) event.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			input->keyUp((Key) event.key.keysym.scancode);
			break;
		case SDL_QUIT:
			graphics->windowQuit();
			break;
		}
	}
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

	SDL_Quit();
}