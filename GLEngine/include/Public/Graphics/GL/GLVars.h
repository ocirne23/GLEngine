#pragma once

#include "Core.h"
#include "3rdparty/rde/rde_string.h"

struct SDL_Window;
typedef void* SDL_GLContext;

class Graphics;

class GLVars
{
public:
	friend class Graphics;

	static uint getGLMajorVersion()                 { return s_glMajorVersion; }
	static uint getGLMinorVersion()                 { return s_glMinorVersion; }
	static uint getMaxTextureUnits()                { return s_maxTextureUnits; }
	static uint getUBOMaxSize()                     { return s_uboMaxSize; }
	static const rde::string& getVendorStr()        { return s_glVendor; }
	static const rde::string& getRendererStr()      { return s_glRenderer; }
	static const rde::string& getDriverVersionStr() { return s_glDriverVersion; }
	static SDL_GLContext getContext()               { return s_glContext; }

private:

	GLVars() {}
	~GLVars() {}
	GLVars(const GLVars& copy) = delete;

	static void createGLContext(SDL_Window* window);
	static void destroyGLContext();

private:

	static SDL_GLContext s_glContext;

	static uint s_glMajorVersion;
	static uint s_glMinorVersion;
	static uint s_maxTextureUnits;
	static uint s_uboMaxSize;
	static uint s_maxTextureSize;

	static rde::string s_glVendor;
	static rde::string s_glRenderer;
	static rde::string s_glDriverVersion;
};