#include "GLEngine.h"

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Network/Network.h"
#include "Utils/ThreadManager.h"

#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

const uint INITIAL_WINDOW_OFFSET_X = 65;
const uint INITIAL_WINDOW_OFFSET_Y = 30;

END_UNNAMED_NAMESPACE()

owner<Input*> GLEngine::input                   = NULL;
owner<Graphics*> GLEngine::graphics             = NULL;
owner<Network*> GLEngine::s_network             = NULL;
bool GLEngine::s_shutdown                       = false;
owner<ThreadManager*> GLEngine::s_threadManager = NULL;

void GLEngine::initialize(const char* a_windowName, uint a_width, uint a_height, EWindowMode a_windowMode)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		print("Unable to initialize SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}
	s_threadManager = new ThreadManager();
	if (a_windowMode != EWindowMode::NONE)
	{
		graphics = new Graphics(a_windowName, 
		                        a_width, 
		                        a_height, 
		                        INITIAL_WINDOW_OFFSET_X, 
		                        INITIAL_WINDOW_OFFSET_Y, 
		                        a_windowMode);
	}
	input = new Input();
	s_network = new Network();
}

void GLEngine::createThread(const char* a_threadName, std::function<void()> a_func)
{
	s_threadManager->createThread(a_threadName, a_func);
}

void GLEngine::doMainThreadTick()
{
	input->pollEvents();
}

void GLEngine::processInput()
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

void GLEngine::createGLContext()
{
	graphics->createGLContext();
}

void GLEngine::destroyGLContext()
{
	graphics->destroyGLContext();
}

void GLEngine::finish()
{
	waitForAllThreadShutdown();
	SAFE_DELETE(s_network);
	SAFE_DELETE(input);
	SAFE_DELETE(graphics);
	SAFE_DELETE(s_threadManager);
	SDL_Quit();
}

void GLEngine::waitForAllThreadShutdown()
{
	s_threadManager->waitForAllThreadShutdown();
}

size_t GLEngine::getStackUsage()
{
	return ThreadManager::getStackUsage();
}
