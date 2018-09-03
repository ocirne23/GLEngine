#include "Window.h"

#include <SDL/SDL_video.h>
#include <SDL/SDL_syswm.h>
#include <Windows.h>

#include "GL/GLContext.h"

owner<IContext*> Window::createContext(const EContextType& type)
{
#ifdef DEBUG_BUILD
	md_numContextsCreated++;
#endif
	switch (type)
	{
	case EContextType::OPENGL:
		return owner<GLContext*>(new GLContext(*this));
	default:
		assert(false);
		return nullptr;
	}
}

void Window::destroyContext(owner<IContext*> context)
{
#ifdef DEBUG_BUILD
	md_numContextsCreated--;
#endif
	switch (context->getType())
	{
	case EContextType::OPENGL:
		delete static_cast<GLContext*>(context);
		break;
	default:
		assert(false);
		break;
	}
}

Window::Window(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
	: m_width(width)
	, m_height(height)
	, m_xPos(xPos)
	, m_yPos(yPos)
{
	uint flags = SDL_WINDOW_SHOWN;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	flags |= SDL_WINDOW_OPENGL;

	if (mode == EWindowMode::BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	else if (mode == EWindowMode::FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;

	m_window = SDL_CreateWindow(name, xPos, yPos, width, height, flags);

	SDL_SysWMinfo winInfo;
	SDL_GetVersion(&winInfo.version);
	SDL_GetWindowWMInfo(m_window, &winInfo);

	m_hwnd = winInfo.info.win.window;
	m_hinstance = rcast<void*>(GetWindowLongPtr(scast<HWND>(m_hwnd), GWLP_HINSTANCE));
	m_wndproc = rcast<void*>(GetWindowLongPtr(scast<HWND>(m_hwnd), GWLP_WNDPROC));
	m_hdc = GetDC(scast<HWND>(m_hwnd));
}

Window::~Window()
{
	SDL_DestroyWindow(m_window);

#ifdef DEBUG_BUILD
	assert(md_numContextsCreated == 0);
#endif
}
