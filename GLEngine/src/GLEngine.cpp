#include "GLEngine.h"

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Utils/WindowsPlatformData.h"

#include <SDL/SDL.h>

Input* GLEngine::input = NULL;
Graphics* GLEngine::graphics = NULL;
bool GLEngine::s_shutdown = false;
SDL_Thread* GLEngine::s_renderThread = NULL;

void GLEngine::initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		print("%s: %s\n", "Unable to initialize SDL", SDL_GetError());
		SDL_Quit();
		return;
	}
	graphics = new Graphics("GLEngine", 1900 / 2, 1000 / 2, 10, 30); // TODO: move construction args to app
	input = new Input();
}

void GLEngine::createRenderThread(std::function<void()> a_func)
{
	s_renderThread = SDL_CreateThread(&GLEngine::renderThread, "RenderThread", &a_func);
}

int GLEngine::renderThread(void* a_ptr)
{
	std::function<void()> renderLoopFunc = *((std::function<void()>*) a_ptr);
	graphics->createGLContext();
	renderLoopFunc();
	return 0;
}

void GLEngine::doMainThreadTick()
{
	input->pollEvents();
}

void GLEngine::doRenderThreadTick()
{
	input->processEvents();
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
	s_shutdown = true;
	if (SDL_ThreadID() != SDL_GetThreadID(s_renderThread))
	{
		print("Waiting for rendering thread to shut down\n");
		SDL_WaitThread(s_renderThread, NULL);
	}
	SAFE_DELETE(input);
	SAFE_DELETE(graphics);
	SDL_Quit();
}