#pragma once

#include "Graphics/IWindow.h"

class Window : public IWindow
{
public:

	Window(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);
	Window(const Window&) = delete;
	virtual ~Window() override;

	virtual owner<IContext*> createContext(const EContextType& contextType) override;
	virtual void             destroyContext(owner<IContext*>& context) override;
	virtual void             swapBuffers() override;
	virtual SDL_Window*      getSDLWindow() override    { return m_window; }
	virtual void*            getHDC() override          { return m_hdc; }
	virtual void*            getHWND() override         { return m_hwnd; }
	virtual void*            getHINSTANCE() override    { return m_hinstance; }
	virtual void*            getWNDPROC() override      { return m_wndproc; }
	virtual uint             getWidth() const override  { return m_width; }
	virtual uint             getHeight() const override { return m_height; }

private:

	SDL_Window* m_window = NULL;
	void* m_hinstance = NULL;
	void* m_hwnd = NULL;
	void* m_hdc = NULL;
	void* m_wndproc = NULL;

	uint m_width = 0;
	uint m_height = 0;
	uint m_xPos = 0;
	uint m_yPos = 0;
};