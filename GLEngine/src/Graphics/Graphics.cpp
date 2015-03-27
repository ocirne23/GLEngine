#include "Graphics/Graphics.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/GLVars.h"
#include "Graphics/tryEnableARBDebugOutput.h"
#include "rde/rde_string.h"
#include "Utils/CheckGLError.h"
#include "Utils/WindowsPlatformData.h"

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <assert.h>

Graphics::Graphics(const char* a_windowName, uint a_screenWidth, uint a_screenHeight, uint a_screenXPos, uint a_screenYPos)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#ifdef ENABLE_ARB_DEBUG_OUTPUT
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
	m_window = SDL_CreateWindow(a_windowName, a_screenXPos, a_screenYPos, a_screenWidth, a_screenHeight, flags);
	if (!m_window)
	{
		print("Unable to create window: %s\n", SDL_GetError());
	}
#ifdef WINDOWS
	WindowsPlatformData::setWindowHandle(m_window);
#endif
}

Graphics::~Graphics()
{
}

void Graphics::createGLContext()
{
	GLVars::createGLContext(m_window);

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		print("GLEW error: %s\n", glewGetErrorString(res));
		return;
	}
	for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError());

	tryEnableARBDebugOutput();

	SDL_GL_SetSwapInterval(m_vsync);

	if (!m_viewportWidth || !m_viewportHeight)
	{
		int screenWidth, screenHeight;
		SDL_GetWindowSize(m_window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		setViewportSize(screenWidth, screenHeight);
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	setDepthTest(true);
}

void Graphics::clear(const glm::vec4& a_color, bool a_clearColor, bool a_clearDepth)
{
	CHECK_GL_ERROR();

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
	SDL_GL_SwapWindow(m_window);
}

void Graphics::setVsync(bool a_enabled)
{
	SDL_GL_SetSwapInterval(a_enabled);
	m_vsync = a_enabled;
}

void Graphics::setDepthTest(bool a_enabled)
{
	if (a_enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Graphics::destroyWindow()
{
	if (m_window)
	{
		GLVars::dispose();
		SDL_DestroyWindow(m_window);
		m_window = NULL;
	}
}

void Graphics::setWindowTitle(const char* a_title)
{
	SDL_SetWindowTitle(m_window, a_title);
}

void Graphics::setViewportPosition(uint a_viewportXPos, uint a_viewportYPos)
{
	m_viewportXPos = a_viewportXPos;
	m_viewportYPos = a_viewportYPos;
	glViewport(m_viewportXPos, m_viewportYPos, m_viewportWidth, m_viewportHeight);
}

void Graphics::setViewportSize(uint a_viewportWidth, uint a_viewportHeight)
{
	m_viewportWidth = a_viewportWidth;
	m_viewportHeight = a_viewportHeight;
	glViewport(m_viewportXPos, m_viewportYPos, m_viewportWidth, m_viewportHeight);
}