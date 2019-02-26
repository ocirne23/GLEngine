#include "GLContext.h"

#include "GL.h"

#include "GL/GLBuffer.h"

#include <assert.h>
#include <Windows.h>

BEGIN_UNNAMED_NAMESPACE()

static const uint MAX_GL_MAJOR_VERSION = 4;
static const uint MAX_GL_MINOR_VERSION = 4;

HGLRC createHighestGLContext(HDC hdc, uint a_maxMajorVersion, uint a_maxMinorVersion)
{
	int contextFlags = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#ifdef _DEBUG
	contextFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#else
	contextFlags |= GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR;
#endif

	HGLRC context = NULL;
	int major = a_maxMajorVersion;
	int minor;
	for (; major >= 0; --major)
	{
		minor = a_maxMinorVersion;
		for (; minor >= 0; --minor)
		{
			int attribs[] =
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
				goto breakLoop;
			}
		}
	}
breakLoop:
	if (!context)
		print("Failed to create OpenGL context\n");
	return context;
}

END_UNNAMED_NAMESPACE()

GLContext::GLContext(void* a_hdc)
{
	assert(!m_glContext);
	m_glContext = createHighestGLContext(scast<HDC>(a_hdc), MAX_GL_MAJOR_VERSION, MAX_GL_MINOR_VERSION);
	m_glVendor = rcast<const char*>(glGetString(GL_VENDOR));
	m_glRenderer = rcast<const char*>(glGetString(GL_RENDERER));
	m_glDriverVersion = rcast<const char*>(glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, rcast<GLint*>(&m_maxTextureUnits));
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, rcast<GLint*>(&m_uboMaxSize));
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, rcast<GLint*>(&m_maxTextureSize));
	glGetIntegerv(GL_MAJOR_VERSION, rcast<GLint*>(&m_glMajorVersion));
	glGetIntegerv(GL_MINOR_VERSION, rcast<GLint*>(&m_glMinorVersion));
	glGetIntegerv(GL_MAX_SAMPLES, rcast<GLint*>(&m_maxMSAASamples));

	print("Created GL Context: %i.%i\n", m_glMajorVersion, m_glMinorVersion);
	print("Vendor: %s\n", m_glVendor.c_str());
	print("Renderer: %s\n", m_glRenderer.c_str());
	print("DriverVersion: %s\n", m_glDriverVersion.c_str());
	print("Max MSAA samples: %i\n", m_maxMSAASamples);
}

GLContext::~GLContext()
{
	assert(m_glContext);
	wglDeleteContext(scast<HGLRC>(m_glContext));
}

owner<IBuffer*> GLContext::createBuffer(uint64 size, const EBufferUsageType& type)
{
	return new GLBuffer(size, type);
}

void GLContext::destroyBuffer(owner<IBuffer*>& buffer)
{
	delete static_cast<GLBuffer*>(buffer);
	buffer = nullptr;
}
