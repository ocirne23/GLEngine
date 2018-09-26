#include "GLContext.h"

#include "Window.h"
#include "GL/GL.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLConstantBuffer.h"
#include "GLTextureBuffer.h"
#include "GLFramebuffer.h"
#include "GLVertexAttributes.h"
#include "GLShader.h"

#include <SDL/SDL_video.h>
#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 5;

HGLRC createHighestGLContext(HDC hdc, uint a_maxMajorVersion, uint a_maxMinorVersion)
{
	HGLRC context = NULL;

	int contextFlags = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#ifdef DEBUG_BUILD
	contextFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#else
	contextFlags |= GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR;
#endif

	for (int major = a_maxMajorVersion; major >= 0; --major)
	{
		for (int minor = a_maxMinorVersion; minor >= 0; --minor)
		{
			const int attribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, major,
				WGL_CONTEXT_MINOR_VERSION_ARB, minor,
				WGL_CONTEXT_FLAGS_ARB, contextFlags,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};
			context = wglCreateContextAttribsARB(hdc, 0, attribs);
			if (context)
			{
				BOOL result = wglMakeCurrent(hdc, context);
				assert(result);
				return context;
			}
		}
	}
	if (!context)
		printf("Failed to create OpenGL context\n");
	return context;
}

END_UNNAMED_NAMESPACE()

GLContext::GLContext(Window& window)
{
	SDL_GLContext context = SDL_GL_CreateContext(window.getSDLWindow());
	glewInit();
	glewExperimental = GL_TRUE;
	const GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		printf("GLEW error: %s\n", glewGetErrorString(res));
		return;
	}
	SDL_GL_DeleteContext(context);

	m_context = createHighestGLContext(scast<HDC>(window.getHDC()), MAX_GL_MAJOR_VERSION, MAX_GL_MINOR_VERSION);

	for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError())
	{
		printf("GLEW error: %s\n", glewGetErrorString(glErr));
	};

	setViewportSize(window.getWidth(), window.getHeight());
	clearColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), true);
}

GLContext::~GLContext()
{
#ifdef DEBUG_BUILD
	assert(md_numBuffersCreated == 0);
	assert(md_numFramebuffersCreated == 0);
	assert(md_numVertexAttributesCreated == 0);
#endif
	assert(m_context);
	wglDeleteContext(scast<HGLRC>(m_context));
}

void GLContext::setViewportSize(uint width, uint height)
{
	glViewport(0, 0, width, height);
}

void GLContext::clearColor(const glm::vec4& color, bool clearDepth)
{
	const GLbitfield flags = clearDepth ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : GL_COLOR_BUFFER_BIT;
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(flags);
}

owner<IBuffer*> GLContext::createBuffer(const EBufferType& type)
{
#ifdef DEBUG_BUILD
	md_numBuffersCreated++;
#endif
	switch (type)
	{
	case EBufferType::VERTEX:
		return new GLVertexBuffer();
	case EBufferType::INDEX:
		return new GLIndexBuffer();
	case EBufferType::CONSTANT:
		return new GLConstantBuffer();
	case EBufferType::TEXBUF:
		return new GLTextureBuffer();
	default:
		assert(false);
		break;
	}
	return nullptr;
}

void GLContext::destroyBuffer(owner<IBuffer*> buffer)
{
#ifdef DEBUG_BUILD
	md_numBuffersCreated--;
#endif
	switch (buffer->getType())
	{
	case EBufferType::VERTEX:
		delete scast<GLVertexBuffer*>(buffer);
		break;
	case EBufferType::INDEX:
		delete scast<GLIndexBuffer*>(buffer);
		break;
	case EBufferType::CONSTANT:
		delete scast<GLConstantBuffer*>(buffer);
		break;
	case EBufferType::TEXBUF:
		delete scast<GLTextureBuffer*>(buffer);
		break;
	default:
		assert(false);
		break;
	}
}

owner<IFramebuffer*> GLContext::createFramebuffer()
{
#ifdef DEBUG_BUILD
	md_numFramebuffersCreated++;
#endif
	return owner<IFramebuffer*>();
}

void GLContext::destroyFramebuffer(owner<IFramebuffer*> framebuffer)
{
#ifdef DEBUG_BUILD
	md_numFramebuffersCreated--;
#endif
	assert(framebuffer);
	delete scast<GLFramebuffer*>(framebuffer);
}

owner<IVertexAttributes*> GLContext::createVertexAttributes()
{
#ifdef DEBUG_BUILD
	md_numVertexAttributesCreated++;
#endif
	return new GLVertexAttributes();
}

void GLContext::destroyVertexAttributes(owner<IVertexAttributes*> vertexAttributes)
{
#ifdef DEBUG_BUILD
	md_numVertexAttributesCreated--;
#endif
	assert(vertexAttributes);
	delete scast<GLVertexAttributes*>(vertexAttributes);
}

owner<IShader*> GLContext::createShader()
{
#ifdef DEBUG_BUILD
	md_numShadersCreated++;
#endif
	return new GLShader();
}

void GLContext::destroyShader(owner<IShader*> shader)
{
#ifdef DEBUG_BUILD
	md_numShadersCreated--;
#endif
	assert(shader);
	delete scast<GLShader*>(shader);
}