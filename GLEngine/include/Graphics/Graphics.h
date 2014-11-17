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

	SDL_Window* getWindow()							{ return m_window; }
	bool hasWindow() const							{ return m_window != NULL; }

	uint getScreenWidth() const						{ return m_screenWidth; }
	uint getScreenHeight() const					{ return m_screenHeight; }
	Viewport getViewport() const					{ return m_viewport; }
	uint getGLMajorVersion() const					{ return m_glMajorVersion; }
	uint getGLMinorVersion() const					{ return m_glMinorVersion; }
	const rde::string& getVendorStr() const			{ return m_glVendor; }
	const rde::string& getRendererStr() const		{ return m_glRenderer; }
	const rde::string& getDriverVersionStr() const	{ return m_glDriverVersion; }
	uint getMaxTextureUnits() const					{ return m_maxTextureUnits; }
	bool getVsyncEnabled() const					{ return m_vsyncEnabled; }

private:
	Graphics() :
		m_window(0), m_screenWidth(0), m_screenHeight(0),
		m_vsyncEnabled(false), m_maxTextureUnits(0), 
		m_glMajorVersion(0), m_glMinorVersion(0), m_glContext(0) 
	{}
	Graphics(const Graphics& copy) {}
	~Graphics() {}

	bool initialize(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, WindowFlags flags);
	void windowQuit();
	void resizeScreen(uint screenWidth, uint screenHeight);

	SDL_GLContext getGLContext();
	SDL_GLContext createGLContext(uint majorVersion, uint minorVersion);
	void disposeGLContext(SDL_GLContext context);
	void createHighestGLContext(uint maxMajorVersion, uint maxMinorVersion);
	void initGLEW();

private:

	SDL_Window* m_window;
	uint m_screenWidth;
	uint m_screenHeight;
	Viewport m_viewport;

	bool m_vsyncEnabled;
	uint m_maxTextureUnits;

	uint m_glMajorVersion;
	uint m_glMinorVersion;
	SDL_GLContext m_glContext;

	rde::string m_glVendor;
	rde::string m_glRenderer;
	rde::string m_glDriverVersion;

	rde::vector<WindowEventListener*> m_windowEventListeners;
};