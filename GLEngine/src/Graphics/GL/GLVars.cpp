#include "Graphics/GL/GLVars.h"

#include "Graphics/GL/GL.h"

#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 5;

SDL_GLContext createHighestGLContext(SDL_Window* a_window, uint& a_outMaxMajorVersion, uint& a_outMaxMinorVersion)
{
	SDL_GLContext context = NULL;

	int major = MAX_GL_MAJOR_VERSION;
	int minor;
	for (; major >= 0; --major)
	{
		minor = MAX_GL_MINOR_VERSION;
		for (; minor >= 0; --minor)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
			context = SDL_GL_CreateContext(a_window);
			if (context)
				goto breakLoop;
		}
	}
breakLoop:

	if (!context)
	{
		print("Failed to create OpenGL context\n");
		a_outMaxMajorVersion = 0;
		a_outMaxMinorVersion = 0;
		return NULL;
	}

	a_outMaxMajorVersion = major;
	a_outMaxMinorVersion = minor;
	return context;
}

END_UNNAMED_NAMESPACE()

SDL_GLContext GLVars::s_glContext = NULL;

uint GLVars::s_glMajorVersion = 0;
uint GLVars::s_glMinorVersion = 0;
uint GLVars::s_maxTextureUnits = 0;
uint GLVars::s_uboMaxSize = 0;
uint GLVars::s_maxTextureSize = 0;

rde::string GLVars::s_glVendor;
rde::string GLVars::s_glRenderer;
rde::string GLVars::s_glDriverVersion;

void GLVars::createGLContext(SDL_Window* a_window)
{
	s_glContext = createHighestGLContext(a_window, s_glMajorVersion, s_glMinorVersion);
	s_glVendor = (const char*) glGetString(GL_VENDOR);
	s_glRenderer = (const char*) glGetString(GL_RENDERER);
	s_glDriverVersion = (const char*) glGetString(GL_VERSION);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*) &s_maxTextureUnits);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*) &s_uboMaxSize);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &s_maxTextureSize);

	print("Created GL Context: %i %i\n", s_glMajorVersion, s_glMinorVersion);
	print("Vendor: %s\n", s_glVendor.c_str());
	print("Max texture size: %i\n", s_maxTextureSize);
}

void GLVars::deleteGLContext()
{
	SDL_GL_DeleteContext(s_glContext);
}