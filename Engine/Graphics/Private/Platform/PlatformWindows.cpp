#include "PlatformWindows.h"

#include "Core/WindowsInclude.h"

#include "GL/GL.h"
#include <GLEW/wglew.h>

#include <stdio.h>
#include <assert.h>

namespace WinPlatform
{

void* createHighestGLContext(void* a_hdc, int a_maxMajorVersion, int a_maxMinorVersion)
{
	HDC hdc = static_cast<HDC>(a_hdc);
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
			printf("Failed to create OpenGL context\n");
		return context;
}

void deleteContext(void* context)
{
	wglDeleteContext(static_cast<HGLRC>(context));
}

void* getHINSTANCE(void* hwnd)
{
	return reinterpret_cast<void*>(GetWindowLongPtr(static_cast<HWND>(hwnd), GWLP_HINSTANCE));
}

void* getWNDPROC(void* hwnd)
{
	return reinterpret_cast<void*>(GetWindowLongPtr(static_cast<HWND>(hwnd), GWLP_WNDPROC));
}

void* getHDC(void* hwnd)
{
	return GetDC(static_cast<HWND>(hwnd));
}

void swapBuffers(void* hdc)
{
	SwapBuffers(static_cast<HDC>(hdc));
}

} // namespace WinPlatform