#include "Graphics/Graphics.h"

#include "Graphics/WindowEventListener.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLTypes.h"
#include "Utils/CheckGLError.h"
#include "rde/rde_string.h"

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include <assert.h>


#ifdef ANDROID
static const uint MAX_GL_MAJOR_VERSION = 3;
static const uint MAX_GL_MINOR_VERSION = 0;
#else

#include "Graphics/tryEnableARBDebugOutput.h"

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 5;

#endif // ANDROID

bool Graphics::initialize(const char* a_windowName, uint a_screenWidth, uint a_screenHeight, uint a_screenXPos, uint a_screenYPos, WindowFlags a_flags)
{
	m_window = SDL_CreateWindow(a_windowName, a_screenXPos, a_screenYPos, a_screenWidth, a_screenHeight, a_flags);
	if (!m_window)
	{
		print("Unable to create window: %s\n", SDL_GetError());
		return false;
	}

	createHighestGLContext(MAX_GL_MAJOR_VERSION, MAX_GL_MINOR_VERSION);
	SDL_GL_SetSwapInterval(m_vsyncEnabled);	//1 is vsync 0 is uncapped

	SDL_GetWindowSize(m_window, (int*)&m_screenWidth, (int*)&m_screenHeight);
	resizeScreen(m_screenWidth, m_screenHeight);
	glViewport(0, 0, m_screenWidth, m_screenHeight);

#ifndef ANDROID
	initGLEW();
	tryEnableARBDebugOutput();
#endif //ANDROID

	m_glVendor = (const char*) glGetString(GL_VENDOR);
	m_glRenderer = (const char*) glGetString(GL_RENDERER);
	m_glDriverVersion = (const char*) glGetString(GL_VERSION);
	m_viewport = { 0, 0, m_screenWidth, m_screenHeight };
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*) &m_maxTextureUnits);

	GLint uboMaxSize;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboMaxSize);

	print("OpenGL context version: %i.%i \nvendor: %s \nGPU: %s \nversion: %s\n", m_glMajorVersion, m_glMinorVersion, m_glVendor.c_str(), m_glRenderer.c_str(), m_glDriverVersion.c_str());
	print("Max texture units %u \n", m_maxTextureUnits);
	print("UBO max size: %i \n", uboMaxSize);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void Graphics::clear(const glm::vec4& a_color, bool a_clearColor, bool a_clearDepth)
{
	CHECK_GL_ERROR();

	assert(m_window);
	glClearColor(a_color.r, a_color.g, a_color.b, a_color.a);
	uint clearFlags = 0;
	if (a_clearColor)
		clearFlags |= GL_COLOR_BUFFER_BIT;
	if (a_clearDepth)
		clearFlags |= GL_DEPTH_BUFFER_BIT;
	glClear(clearFlags);
}

void Graphics::swap()
{
	assert(m_window);
	SDL_GL_SwapWindow(m_window);
}

void Graphics::setVsync(bool a_enabled)
{
	assert(m_window);
	SDL_GL_SetSwapInterval(a_enabled);
	m_vsyncEnabled = a_enabled;
}

void Graphics::registerWindowEventListener(WindowEventListener* a_listener)
{
	m_windowEventListeners.push_back(a_listener);
}

void Graphics::unregisterWindowEventListener(WindowEventListener* a_listener)
{
	m_windowEventListeners.erase(m_windowEventListeners.find(a_listener));
}

void Graphics::resizeScreen(uint a_screenWidth, uint a_screenHeight)
{
	assert(m_window);
	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;

	m_viewport = { 0, 0, a_screenWidth, a_screenHeight };
	glViewport(0, 0, a_screenWidth, a_screenHeight);

	for (WindowEventListener* listener : m_windowEventListeners)
		listener->resize(a_screenWidth, a_screenHeight);
}

void Graphics::windowQuit()
{
	assert(m_window);
	for (WindowEventListener* listener : m_windowEventListeners)
		listener->quit();
}

void Graphics::destroyWindow()
{
	assert(m_window);
	if (m_window)
	{
		disposeGLContext(m_glContext);
		SDL_DestroyWindow(m_window);
		m_window = NULL;
	}
}

void Graphics::setWindowTitle(const char* a_title)
{
	assert(m_window);
	SDL_SetWindowTitle(m_window, a_title);
}

SDL_GLContext Graphics::createGLContext(uint a_majorVersion, uint a_minorVersion)
{
	assert(m_window);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, a_majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, a_minorVersion);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifdef ENABLE_ARB_DEBUG_OUTPUT
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	return SDL_GL_CreateContext(m_window);
}

void Graphics::createHighestGLContext(uint maxMajorVersion, uint maxMinorVersion)
{
	int major = maxMajorVersion;
	int minor;
	for (; major >= 0; --major)
	{
		minor = maxMinorVersion;
		for (; minor >= 0; --minor)
		{
			m_glContext = createGLContext(major, minor);
			if (m_glContext)
				goto breakLoop;
		}
	}
breakLoop:

	if (!m_glContext)
	{
		print("Failed to create OpenGL context\n");
		m_glMajorVersion = 0;
		m_glMinorVersion = 0;
		return;
	}

	m_glMajorVersion = major;
	m_glMinorVersion = minor;
}

void Graphics::initGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		print("GLEW error: %s\n", glewGetErrorString(err));
	}
	else
	{
		for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError());
	}
}

void Graphics::disposeGLContext(SDL_GLContext a_context)
{
	assert(m_window);
	SDL_GL_DeleteContext(a_context);
}