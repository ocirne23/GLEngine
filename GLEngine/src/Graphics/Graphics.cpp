#include "Graphics/Graphics.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/GLVars.h"
#include "rde/rde_string.h"
#include "Utils/CheckGLError.h"

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include <assert.h>

#include "Graphics/tryEnableARBDebugOutput.h"

bool Graphics::initialize(const char* a_windowName, uint a_screenWidth, uint a_screenHeight, uint a_screenXPos, uint a_screenYPos, WindowFlags a_flags)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#ifdef ENABLE_ARB_DEBUG_OUTPUT
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	m_window = SDL_CreateWindow(a_windowName, a_screenXPos, a_screenYPos, a_screenWidth, a_screenHeight, a_flags);
	if (!m_window)
	{
		print("Unable to create window: %s\n", SDL_GetError());
		return false;
	}

	GLVars::init(m_window);

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		print("GLEW error: %s\n", glewGetErrorString(res));
		return false;
	}
	for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError());
	
	tryEnableARBDebugOutput();

	SDL_GL_SetSwapInterval(m_vsyncEnabled);

	SDL_GetWindowSize(m_window, (int*)&m_screenWidth, (int*)&m_screenHeight);
	glViewport(0, 0, m_screenWidth, m_screenHeight);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	setDepthTest(true);

	resizeScreen(m_screenWidth, m_screenHeight);

	return true;
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
	m_vsyncEnabled = a_enabled;
}

void Graphics::setDepthTest(bool a_enabled)
{
	if (a_enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Graphics::resizeScreen(uint a_screenWidth, uint a_screenHeight)
{
	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;

	glViewport(0, 0, a_screenWidth, a_screenHeight);

	for (auto& listener : m_windowResizeListeners)
		listener.second(a_screenWidth, a_screenHeight);
}

void Graphics::windowQuit()
{
	for (auto& listener : m_windowQuitListeners)
		listener.second();
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

void Graphics::registerWindowResizeListener(void* ownerPtr, std::function<void(float, float)> func)
{
	m_windowResizeListeners.insert({ ownerPtr, func });
}

void Graphics::unregisterWindowResizeListener(void* ownerPtr)
{
	m_windowResizeListeners.erase(ownerPtr);
}

void Graphics::registerWindowQuitListener(void* ownerPtr, std::function<void()> func)
{
	m_windowQuitListeners.insert({ ownerPtr, func });
}

void Graphics::unregisterWindowQuitListener(void* ownerPtr)
{
	m_windowQuitListeners.erase(ownerPtr);
}
