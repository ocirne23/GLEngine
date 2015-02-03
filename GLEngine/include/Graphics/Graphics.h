#pragma once

#include "Core.h"

#include "Utils/Viewport.h"
#include "Utils/VecForward.h"

#include "rde/rde_string.h"
#include "rde/vector.h"

struct SDL_Window;
class WindowEventListener;

typedef void* SDL_GLContext;
typedef uint WindowFlags;

class Graphics
{
public:
	friend class GLEngine;

	void clear(const glm::vec4& color, bool clearColor = true, bool clearDepth = true);
	void swap();

	void registerWindowEventListener(WindowEventListener* listener);
	void unregisterWindowEventListener(WindowEventListener* listener);
	void destroyWindow();

	void setWindowTitle(const char* title);
	void setVsync(bool enabled);

	SDL_Window* getWindow()			{ return m_window; }
	bool hasWindow() const			{ return m_window != NULL; }

	uint getScreenWidth() const		{ return m_screenWidth; }
	uint getScreenHeight() const	{ return m_screenHeight; }
	Viewport getViewport() const	{ return m_viewport; }
	bool getVsyncEnabled() const	{ return m_vsyncEnabled; }

private:
	Graphics() :
		m_window(0), m_screenWidth(0), m_screenHeight(0), m_vsyncEnabled(false)
	{}
	Graphics(const Graphics& copy) {}
	~Graphics() {}

	bool initialize(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, WindowFlags flags);
	void windowQuit();
	void resizeScreen(uint screenWidth, uint screenHeight);

private:

	SDL_Window* m_window;
	uint m_screenWidth;
	uint m_screenHeight;
	bool m_vsyncEnabled;

	Viewport m_viewport;
	rde::vector<WindowEventListener*> m_windowEventListeners;
};