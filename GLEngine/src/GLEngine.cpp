#include "GLEngine.h"

#include "Graphics/Graphics.h"
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
bool GLEngine::s_shutdown = false;
SDL_Thread* GLEngine::s_renderThread = NULL;

void GLEngine::createRenderThread(std::function<void()> a_func)
{
	s_renderThread = SDL_CreateThread(&GLEngine::renderThread, "RenderThread", &a_func);
}

void GLEngine::doMainThreadTick()
{
	input->pollEvents();
}

void GLEngine::doRenderThreadTick()
{
	input->processEvents();
}

int GLEngine::renderThread(void* a_ptr)
{
	std::function<void()> renderLoopFunc = *((std::function<void()>*) a_ptr);
	graphics->createGLContext();
	renderLoopFunc();
	dispose();
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

	graphics = new Graphics(PROGRAM_NAME,
							INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT,
							INIT_WINDOW_XPOS, INIT_WINDOW_YPOS);
	input = new Input();
}

void GLEngine::sleep(uint a_timeMs)
{
	SDL_Delay(a_timeMs);
}

uint GLEngine::getTimeMs()
{
	return SDL_GetTicks();
}

void GLEngine::dispose()
{
	SAFE_DELETE(input);
	SAFE_DELETE(graphics);
	SDL_Quit();
}

void GLEngine::shutdown()
{
	s_shutdown = true;
	if (SDL_ThreadID() != SDL_GetThreadID(s_renderThread))
	{
		print("Waiting for rendering thread to shut down\n");
		SDL_WaitThread(s_renderThread, NULL);
	}
}