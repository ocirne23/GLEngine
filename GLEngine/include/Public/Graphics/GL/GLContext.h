#pragma once

#include "Core.h"
#include "EASTL/string.h"

struct SDL_Window;
typedef void* ContextHandle;

class Graphics;

class GLContext
{
public:
	friend class Graphics;

	~GLContext();

	uint getGLMajorVersion() const                   { return m_glMajorVersion; }
	uint getGLMinorVersion() const                   { return m_glMinorVersion; }
	uint getMaxTextureUnits() const                  { return m_maxTextureUnits; }
	uint getUBOMaxSize() const                       { return m_uboMaxSize; }
	const eastl::string& getVendorStr() const        { return m_glVendor; }
	const eastl::string& getRendererStr() const      { return m_glRenderer; }
	const eastl::string& getDriverVersionStr() const { return m_glDriverVersion; }
	const ContextHandle getSDLContext() const        { return m_glContext; }

private:

	GLContext(void* hdc);
	GLContext(const GLContext& copy) = delete;

private:

	ContextHandle m_glContext = NULL;

	uint m_glMajorVersion  = 0;
	uint m_glMinorVersion  = 0;
	uint m_maxTextureUnits = 0;
	uint m_uboMaxSize      = 0;
	uint m_maxTextureSize  = 0;
	uint m_maxMSAASamples  = 0;

	eastl::string m_glVendor;
	eastl::string m_glRenderer;
	eastl::string m_glDriverVersion;
};