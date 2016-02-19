#include "Graphics/Graphics.h"

#include "GLEngine.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLContext.h"
#include "Graphics/Utils/ARBDebugOutput.h"
#include "Graphics/Utils/CheckGLError.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include <glm/glm.hpp>
#include <SDL/SDL_video.h>
#include <SDL/SDL_syswm.h>

#include <Windows.h>
#include <assert.h>

Graphics::Graphics(const char* a_windowName, uint a_screenWidth, uint a_screenHeight, uint a_screenXPos, uint a_screenYPos, const EWindowMode& a_windowMode)
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	/* Does not seem to be required?
	if (GLConfig::getMultisampleType() != GLFramebuffer::EMultiSampleType::NONE)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	}
	*/
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	uint flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (a_windowMode == EWindowMode::BORDERLESS)
		flags |= SDL_WINDOW_BORDERLESS;
	else if (a_windowMode == EWindowMode::FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;
	m_window = SDL_CreateWindow(a_windowName, a_screenXPos, a_screenYPos, a_screenWidth, a_screenHeight, flags);

	SDL_SysWMinfo winInfo;
	SDL_GetVersion(&winInfo.version);
	SDL_GetWindowWMInfo(m_window, &winInfo);

	m_hwnd = winInfo.info.win.window;
	m_hinstance = rcast<void*>(GetWindowLongPtr(scast<HWND>(m_hwnd), GWLP_HINSTANCE));
	m_hdc = GetDC(scast<HWND>(m_hwnd));
}

Graphics::~Graphics()
{
	assert(!m_context && "destroyGLContext() was not called");
	SDL_DestroyWindow(m_window);

	SAFE_DELETE(m_context);
}

void Graphics::createGLContext()
{	
	glewExperimental = GL_TRUE;
	
	// Create a temporary context for GLEW
	SDL_GLContext context = SDL_GL_CreateContext(m_window);
	const GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		print("GLEW error: %s\n", glewGetErrorString(res));
		return;
	}
	SDL_GL_DeleteContext(context);

	m_context = new GLContext();
	for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError()) 
	{
		print("GLEW error: %s\n", glewGetErrorString(glErr));
	};

	ARBDebugOutput::tryEnable();
	GLConfig::initialize();
	
	SDL_GL_SetSwapInterval(m_vsync);

	if (!m_viewportWidth || !m_viewportHeight)
	{
		int screenWidth, screenHeight;
		SDL_GetWindowSize(m_window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		setViewportSize(screenWidth, screenHeight);
	}
	
	setFaceCulling(EFaceCulling::BACK);
	setMultisample(GLConfig::getMultisampleType() != GLFramebuffer::EMultiSampleType::NONE);
	setDepthTest(true);

	CHECK_GL_ERROR();
}

void Graphics::destroyGLContext()
{
	SAFE_DELETE(m_context);
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
	SwapBuffers(scast<HDC>(m_hdc));
}

void Graphics::setVsync(bool a_enabled)
{
	wglSwapIntervalEXT(a_enabled);
	m_vsync = a_enabled;
}

void Graphics::setDepthTest(bool a_enabled)
{
	if (a_enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Graphics::setDepthWrite(bool a_enabled)
{
	glDepthMask(a_enabled);
}

void Graphics::setColorWrite(bool a_enabled)
{
	glColorMask(a_enabled, a_enabled, a_enabled, a_enabled);
}

void Graphics::setFaceCulling(EFaceCulling a_face)
{
	if (a_face != EFaceCulling::NONE)
	{
		glEnable(GL_CULL_FACE);
		if (a_face == EFaceCulling::FRONT)
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);
	}
	else
		glDisable(GL_CULL_FACE);
}

void Graphics::setBlending(bool a_enabled)
{
	if (a_enabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);
}

void Graphics::setMultisample(bool a_enabled)
{
	if (a_enabled)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
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

void Graphics::clearDepthOnly()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Graphics::setDepthFunc(EDepthFunc a_depthFunc)
{
	GLenum func;
	switch (a_depthFunc)
	{
	case EDepthFunc::LESS: func = GL_LESS; break;
	case EDepthFunc::LEQUAL: func = GL_LEQUAL; break;
	case EDepthFunc::EQUAL: func = GL_EQUAL; break;
	case EDepthFunc::GREATER: func = GL_GREATER; break;
	default: assert(false);
	}
	glDepthFunc(func);
}