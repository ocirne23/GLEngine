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

	void setWindowTitle(const char* title);
	void setWindowSize(uint screenWidth, uint screenHeight);
	void setWindowMaximumSize(uint maxWidth, uint maxHeight);
	void setWindowPosition(uint xPos, uint yPos);
	void setWindowResizeable(bool resizeable);
	void setWindowFullscreen(bool fullscreen, bool borderless);
	void setViewportSize(uint viewportWidth, uint viewportHeight);
	void setViewportPosition(uint viewportXPos, uint viewportYPos);
	void setVsync(bool enabled);
	void setDepthTest(bool enabled);

	uint getViewportWidth() const  { return m_viewportWidth; }
	uint getViewportHeight() const { return m_viewportHeight; }
	bool getVsyncEnabled() const   { return m_vsync; }

	void destroyWindow();

private:
	Graphics(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos);
	~Graphics();
	Graphics(const Graphics& copy) = delete;

	void initializeGLContext();

private:

	SDL_Window* m_window  = NULL;
	bool m_fullscreen     = false;
	bool m_bordered       = false;
	bool m_vsync          = false;
	uint m_viewportXPos   = 0;
	uint m_viewportYPos   = 0;
	uint m_viewportWidth  = 0;
	uint m_viewportHeight = 0;
};