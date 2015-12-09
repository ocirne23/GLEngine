#include "Graphics/GL/GLContext.h"

#include "Graphics/GL/GL.h"

#include <assert.h>
#include <SDL/SDL.h>

BEGIN_UNNAMED_NAMESPACE()

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 5;

SDL_GLContext createHighestGLContext(SDL_Window* a_window, uint a_maxMajorVersion, uint a_maxMinorVersion)
{
	SDL_GLContext context = NULL;
	int major = a_maxMajorVersion;
	int minor;
	for (; major >= 0; --major)
	{
		minor = a_maxMinorVersion;
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
		print("Failed to create OpenGL context\n");
	return context;
}

END_UNNAMED_NAMESPACE()

GLContext::GLContext(SDL_Window* a_window)
{
	assert(!m_glContext);
	m_glContext = createHighestGLContext(a_window, MAX_GL_MAJOR_VERSION, MAX_GL_MINOR_VERSION);
	m_glVendor = (const char*) glGetString(GL_VENDOR);
	m_glRenderer = (const char*) glGetString(GL_RENDERER);
	m_glDriverVersion = (const char*) glGetString(GL_VERSION);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*) &m_maxTextureUnits);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, (GLint*) &m_uboMaxSize);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*) &m_maxTextureSize);
	glGetIntegerv(GL_MAJOR_VERSION, (GLint*) &m_glMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, (GLint*) &m_glMinorVersion);
	
	print("Created GL Context: %i.%i\n", m_glMajorVersion, m_glMinorVersion);
	print("Vendor: %s\n", m_glVendor.c_str());
	print("Renderer: %s\n", m_glRenderer.c_str());
	print("DriverVersion: %s\n", m_glDriverVersion.c_str());
}

GLContext::~GLContext()
{
	assert(m_glContext);
	SDL_GL_DeleteContext(m_glContext);
}