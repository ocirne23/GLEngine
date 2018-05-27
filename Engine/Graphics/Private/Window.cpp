#include "Window.h"

#include <SDL/SDL_video.h>
#include <SDL/SDL_syswm.h>
#include <Windows.h>

Window::Window(const char* a_name, uint a_width, uint a_height, uint a_xPos, uint a_yPos, const EWindowMode& a_mode)
	: m_width(a_width)
	, m_height(a_height)
	, m_xPos(a_xPos)
	, m_yPos(a_yPos)
{
	uint flags = SDL_WINDOW_SHOWN;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	flags |= SDL_WINDOW_OPENGL;

	if (a_mode == EWindowMode::BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	else if (a_mode == EWindowMode::FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;

	m_window = SDL_CreateWindow(a_name, a_xPos, a_yPos, a_width, a_height, flags);

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
}
