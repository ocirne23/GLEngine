#include "Graphics/GL/GLContext.h"

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

GLContext::GLContext(SDL_Window* a_window)
{
	assert(!m_glContext);
	m_glContext = createHighestGLContext(a_window, m_glMajorVersion, m_glMinorVersion);
	m_glVendor = (const char*) glGetString(GL_VENDOR);
	m_glRenderer = (const char*) glGetString(GL_RENDERER);
	m_glDriverVersion = (const char*) glGetString(GL_VERSION);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*) &m_maxTextureUnits);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*) &m_uboMaxSize);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &m_maxTextureSize);

	print("Created GL Context: %i %i\n", m_glMajorVersion, m_glMinorVersion);
	print("Vendor: %s\n", m_glVendor.c_str());
	print("Max texture size: %i\n", m_maxTextureSize);
}

GLContext::~GLContext()
{
	assert(m_glContext);
	SDL_GL_DeleteContext(m_glContext);
}