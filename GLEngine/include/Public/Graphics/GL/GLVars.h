#pragma once

#include "Core.h"
#include "rde/rde_string.h"

struct SDL_Window;
typedef void* SDL_GLContext;

class GLVars
{
public:
	static void createGLContext(SDL_Window* window);
	static void dispose();

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

private:

	static SDL_GLContext s_glContext;

	static uint s_glMajorVersion;
	static uint s_glMinorVersion;
	static uint s_maxTextureUnits;
	static uint s_uboMaxSize;

	static rde::string s_glVendor;
	static rde::string s_glRenderer;
	static rde::string s_glDriverVersion;
};