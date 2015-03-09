#pragma once

#include "Core.h"
#include "rde/rde_string.h"
#include "Utils/VecForward.h"
#include "Utils/ListenerMacros.h"

#include <functional>

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

	void destroyWindow();

	void setWindowTitle(const char* title);
	void setVsync(bool enabled);
	void setDepthTest(bool enabled);

	SDL_Window* getWindow()			{ return m_window; }
	bool hasWindow() const			{ return m_window != NULL; }

	uint getScreenWidth() const		{ return m_screenWidth; }
	uint getScreenHeight() const		{ return m_screenHeight; }
	bool getVsyncEnabled() const		{ return m_vsyncEnabled; }

	void windowResize(uint screenWidth, uint screenHeight);

private:
	Graphics(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, WindowFlags flags);
	~Graphics();
	Graphics(const Graphics& copy) = delete;

	void initializeGLContext();

private:

	SDL_Window* m_window	= NULL;
	uint m_screenWidth		= 0;
	uint m_screenHeight		= 0;
	bool m_vsyncEnabled		= false;
};