//#include <SDL/SDL_video.h>
//#include <SDL/SDL_syswm.h>

#include "Core.h"
#include "Platform/PlatformWindows.h"

export module Graphics.Window;

import Graphics.IContext;
import Graphics.GLContext;
import Graphics.IWindow;

export class Window final : public IWindow
{
public:

	Window(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
		: m_width(width)
		, m_height(height)
		, m_xPos(xPos)
		, m_yPos(yPos)
	{
		/*
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
		SDL_GetWindowSize(m_window, acast<int*>(&m_width), acast<int*>(&m_height));

		m_hwnd = winInfo.info.win.window;
		m_hinstance = WinPlatform::getHINSTANCE(m_hwnd);
		m_wndproc = WinPlatform::getWNDPROC(m_hwnd);
		m_hdc = WinPlatform::getHDC(m_hwnd);*/
	}

	Window(const Window&) = delete;

	virtual ~Window() override
	{
		//SDL_DestroyWindow(m_window);
	}

	virtual owner<IContext*> createContext(const EContextType& contextType) override
	{
		switch (contextType)
		{
		case EContextType::OPENGL:
			return new GLContext(m_hdc);
		default:
			assert(false);
			return nullptr;
		}
	}

	virtual void destroyContext(owner<IContext*>& context) override
	{
		switch (context->getContextType())
		{
		case EContextType::OPENGL:
			delete static_cast<GLContext*>(context);
		default:
			assert(false);
		}
		context = nullptr;
	}

	virtual void swapBuffers() override
	{
		WinPlatform::swapBuffers(m_hdc);
	}

	virtual void*            getSDLWindow() override    { return m_window; }
	virtual void*            getHDC() override          { return m_hdc; }
	virtual void*            getHWND() override         { return m_hwnd; }
	virtual void*            getHINSTANCE() override    { return m_hinstance; }
	virtual void*            getWNDPROC() override      { return m_wndproc; }
	virtual uint             getWidth() const override  { return m_width; }
	virtual uint             getHeight() const override { return m_height; }

private:

	void* m_window       = NULL;
	void* m_hinstance    = NULL;
	void* m_hwnd         = NULL;
	void* m_hdc          = NULL;
	void* m_wndproc      = NULL;
	uint m_width         = 0;
	uint m_height        = 0;
	uint m_xPos          = 0;
	uint m_yPos          = 0;
};