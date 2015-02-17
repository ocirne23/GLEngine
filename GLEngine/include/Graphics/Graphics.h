#pragma once

#include "Core.h"

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
	void setDepthTest(bool enabled);

	SDL_Window* getWindow()			{ return m_window; }
	bool hasWindow() const			{ return m_window != NULL; }

	uint getScreenWidth() const		{ return m_screenWidth; }
	uint getScreenHeight() const		{ return m_screenHeight; }
	bool getVsyncEnabled() const		{ return m_vsyncEnabled; }

private:
	Graphics() {}
	~Graphics() {}
	Graphics(const Graphics& copy) = delete;

	bool initialize(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, WindowFlags flags);
	void windowQuit();
	void resizeScreen(uint screenWidth, uint screenHeight);

private:

	SDL_Window* m_window	= NULL;
	uint m_screenWidth		= 0;
	uint m_screenHeight		= 0;
	bool m_vsyncEnabled		= false;

	rde::vector<WindowEventListener*> m_windowEventListeners;
};