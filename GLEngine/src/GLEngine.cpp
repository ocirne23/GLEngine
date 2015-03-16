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
bool GLEngine::s_shutdown				= false;
SDL_Thread* GLEngine::s_renderThread	= NULL;

#ifdef EDITOR
Editor* GLEngine::editor = NULL;
#endif // EDITOR

void GLEngine::createRenderThread(std::function<void()> a_func)
{
	s_renderThread = SDL_CreateThread(&GLEngine::renderThread, "RenderThread", &a_func);
}

void GLEngine::doMainThreadTick()
{
	input->pollEvents();

#ifdef EDITOR
	editor->updateUIPosition();
#endif // EDITOR
}

void GLEngine::doRenderThreadTick()
{
	input->processEvents();
}

int GLEngine::renderThread(void* a_ptr)
{
	std::function<void()> func = *((std::function<void()>*) a_ptr);
	graphics->initializeGLContext();
	func();
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
		INIT_WINDOW_XPOS, INIT_WINDOW_YPOS,
		(WINDOW_OPENGL | WINDOW_SHOWN | WINDOW_RESIZABLE));
	input = new Input();
#ifdef EDITOR
	editor = new Editor();
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

void GLEngine::dispose()
{
	SAFE_DELETE(input);
	SAFE_DELETE(graphics);
#ifdef EDITOR
	editor->quit();
	SAFE_DELETE(editor);
#endif // EDITOR
	SDL_Quit();
}

void GLEngine::shutdown()
{
	s_shutdown = true;
	
	print("da: %i : %i \n", SDL_ThreadID(), SDL_GetThreadID(s_renderThread));

	if (SDL_ThreadID() != SDL_GetThreadID(s_renderThread))
	{
		print("Waiting for rendering thread to shut down\n");
		SDL_WaitThread(s_renderThread, NULL);
	}
}