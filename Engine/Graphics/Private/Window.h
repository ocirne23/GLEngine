#pragma once

#include "Graphics/IWindow.h"

struct SDL_Window;

class Window final : public IWindow
{
public:

	virtual owner<IContext*> createContext(const EContextType& type) override;
	virtual void destroyContext(owner<IContext*> context) override;

private:

	friend class Graphics;
	Window(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);
	Window(const Window&) = delete;
	~Window() override;

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