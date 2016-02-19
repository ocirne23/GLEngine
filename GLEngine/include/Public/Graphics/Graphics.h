#pragma once

#include "Core.h"
#include "Utils/VecForward.h"
#include "gsl/gsl.h"

typedef uint WindowFlags;

struct SDL_Window;
struct SDL_SysWMinfo;
class WindowEventListener;
class GLContext;
enum class EWindowMode;

class Graphics
{
public:
	friend class GLEngine;
	enum class EFaceCulling { FRONT, BACK, NONE };
	enum class EDepthFunc { LESS, LEQUAL, EQUAL, GREATER };

	void clear(const glm::vec4& color, bool clearColor = true, bool clearDepth = true);
	void swap();

	void setWindowTitle(const char* title);
	//TODO
	//void setWindowSize(uint screenWidth, uint screenHeight);
	//void setWindowMaximumSize(uint maxWidth, uint maxHeight);
	//void setWindowPosition(uint xPos, uint yPos);
	//void setWindowResizeable(bool resizeable);
	//void setWindowFullscreen(bool fullscreen, bool borderless);
	void setViewportSize(uint viewportWidth, uint viewportHeight);
	void setViewportPosition(uint viewportXPos, uint viewportYPos);
	void setVsync(bool enabled);
	void setDepthTest(bool enabled);
	void setDepthFunc(EDepthFunc depthFunc);
	void setDepthWrite(bool enabled);
	void setColorWrite(bool enabled);
	void setFaceCulling(EFaceCulling face);
	void setBlending(bool enabled); //TODO blend func vars
	void setMultisample(bool enabled);
	void clearDepthOnly();

	uint getViewportWidth() const  { return m_viewportWidth; }
	uint getViewportHeight() const { return m_viewportHeight; }
	bool getVsyncEnabled() const   { return m_vsync; }
	void* getHWND() const          { return m_hwnd; }
	void* getHINSTANCE() const     { return m_hinstance; }
	void* getHDC() const           { return m_hdc; }

private:

	Graphics(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, const EWindowMode& windowMode);
	~Graphics();
	Graphics(const Graphics& copy) = delete;

	void createGLContext();
	void destroyGLContext();

private:
	
	owner<GLContext*> m_context = NULL;
	SDL_Window* m_window        = NULL;
	void* m_hinstance           = NULL;
	void* m_hwnd                = NULL;
	void* m_hdc                 = NULL;
	bool m_fullscreen           = false;
	bool m_bordered             = false;
	bool m_vsync                = false;
	uint m_viewportXPos         = 0;
	uint m_viewportYPos         = 0;
	uint m_viewportWidth        = 0;
	uint m_viewportHeight       = 0;
};