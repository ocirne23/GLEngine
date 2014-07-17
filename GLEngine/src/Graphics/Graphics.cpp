#include "Graphics\Graphics.h"

#include "Core.h"
#include "Graphics\GL\GL.h"
#include "Graphics\WindowEventListener.h"
#include "rde\rde_string.h"

#include "Utils\CheckGLError.h"

#include <assert.h>
#include <glm\glm.hpp>
#include <SDL\SDL.h>
#include <SDL\SDL_syswm.h>

#ifdef ANDROID

static const uint MAX_GL_MAJOR_VERSION = 3;
static const uint MAX_GL_MINOR_VERSION = 0;

#else

#define ENABLE_ARB_DEBUG_OUTPUT
#include "Graphics\ARBDebugOutput.h"

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 4;

#endif // ANDROID

bool Graphics::initialize(const char* windowName, uint screenWidth, uint screenHeight, uint screenXPos, uint screenYPos, WindowFlags flags)
{
	m_window = SDL_CreateWindow(windowName, screenXPos, screenYPos, screenWidth, screenHeight, flags);
	if (!m_window)
	{
		print("Unable to create window: %s\n", SDL_GetError());
		return false;
	}

	SDL_GetWindowSize(m_window, (int*) &m_screenWidth, (int*) &m_screenHeight);

	resizeScreen(m_screenWidth, m_screenHeight);

	int major = MAX_GL_MAJOR_VERSION;
	int minor = MAX_GL_MINOR_VERSION;
	for (; major >= 0; --major)
	{
		minor = MAX_GL_MINOR_VERSION;

		for (; minor >= 0; --minor)
		{
			m_glContext = createGLContext(major, minor);
			if (m_glContext) break;
			//SDL_ClearError();
		}
		if (m_glContext) break;
	}
	if (!m_glContext)
	{
		print("Failed to create OpenGL context\n"); 
		return false;
	}
	CHECK_GL_ERROR();

	m_glMajorVersion = major;
	m_glMinorVersion = minor;

	SDL_GL_SetSwapInterval(m_vsyncEnabled);	//1 is vsync 0 is uncapped
	CHECK_GL_ERROR();

#ifndef ANDROID

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

#ifdef ENABLE_ARB_DEBUG_OUTPUT
	tryEnableARBDebugOutput();
#endif

#endif //ANDROID
	CHECK_GL_ERROR();

	m_glVendor = (const char*) glGetString(GL_VENDOR);
	m_glRenderer = (const char*) glGetString(GL_RENDERER);
	m_glDriverVersion = (const char*) glGetString(GL_VERSION);
	m_viewport = { 0, 0, m_screenWidth, m_screenHeight };

	glViewport(0, 0, m_screenWidth, m_screenHeight);

	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	m_maxTextureUnits = (uint) maxTextureUnits;

	GLint uboMaxSize;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboMaxSize);

	m_textureManager = new GLTextureManager();
	CHECK_GL_ERROR();

	print("OpenGL context version: %i.%i \nvendor: %s \nGPU: %s \nversion: %s\n", major, minor, m_glVendor.c_str(), m_glRenderer.c_str(), m_glDriverVersion.c_str());
	print("Max texture units %u \n", m_maxTextureUnits);
	print("UBO max size: %i \n", uboMaxSize);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void Graphics::clear(const glm::vec4& color, bool clearColor, bool clearDepth)
{
	CHECK_GL_ERROR();

	assert(m_window);
	glClearColor(color.r, color.g, color.b, color.a);
	uint clearFlags = 0;
	if (clearColor)
		clearFlags |= GL_COLOR_BUFFER_BIT;
	if (clearDepth)
		clearFlags |= GL_DEPTH_BUFFER_BIT;
	glClear(clearFlags);
}

void Graphics::swap()
{
	assert(m_window);
	SDL_GL_SwapWindow(m_window);
}

void Graphics::setVsync(bool enabled)
{
	assert(m_window);
	SDL_GL_SetSwapInterval(enabled);
	m_vsyncEnabled = enabled;
}

void Graphics::registerWindowEventListener(WindowEventListener* listener)
{
	m_windowEventListeners.push_back(listener);
}

void Graphics::unregisterWindowEventListener(WindowEventListener* listener)
{
	m_windowEventListeners.erase(m_windowEventListeners.find(listener));
}

void Graphics::resizeScreen(uint screenWidth, uint screenHeight)
{
	assert(m_window);
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_viewport = { 0, 0, screenWidth, screenHeight };
	glViewport(0, 0, screenWidth, screenHeight);

	for (WindowEventListener* listener : m_windowEventListeners)
		listener->resize(screenWidth, screenHeight);
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

void Graphics::setWindowTitle(const char* title)
{
	assert(m_window);
	SDL_SetWindowTitle(m_window, title);
}

SDL_GLContext Graphics::createGLContext(uint majorVersion, uint minorVersion)
{
	assert(m_window);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifdef ENABLE_ARB_DEBUG_OUTPUT
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	SDL_GLContext context = SDL_GL_CreateContext(m_window);
	return context;
}

void Graphics::disposeGLContext(SDL_GLContext context)
{
	assert(m_window);
	SDL_GL_DeleteContext(context);
}