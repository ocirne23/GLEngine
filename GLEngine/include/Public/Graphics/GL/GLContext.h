#pragma once

#include "Core.h"
#include "3rdparty/rde/rde_string.h"

struct SDL_Window;
typedef void* SDL_GLContext;

class Graphics;

class GLContext
{
public:
	friend class Graphics;

	uint getGLMajorVersion() const                 { return m_glMajorVersion; }
	uint getGLMinorVersion() const                 { return m_glMinorVersion; }
	uint getMaxTextureUnits() const                { return m_maxTextureUnits; }
	uint getUBOMaxSize() const                     { return m_uboMaxSize; }
	const rde::string& getVendorStr() const        { return m_glVendor; }
	const rde::string& getRendererStr() const      { return m_glRenderer; }
	const rde::string& getDriverVersionStr() const { return m_glDriverVersion; }
	const SDL_GLContext getSDLContext() const      { return m_glContext; }

private:

	GLContext(SDL_Window* window);
	~GLContext();
	GLContext(const GLContext& copy) = delete;

	void createGLContext(SDL_Window* window);
	void destroyGLContext();

private:

	SDL_GLContext m_glContext = NULL;

	uint m_glMajorVersion = 0;
	uint m_glMinorVersion = 0;
	uint m_maxTextureUnits = 0;
	uint m_uboMaxSize = 0;
	uint m_maxTextureSize = 0;

	rde::string m_glVendor;
	rde::string m_glRenderer;
	rde::string m_glDriverVersion;
};