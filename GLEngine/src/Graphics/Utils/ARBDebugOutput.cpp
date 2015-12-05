#include "Graphics/Utils/ARBDebugOutput.h"

#include "Core.h"
#include "Graphics/GL/GL.h"

#include <SDL/SDL_syswm.h>
#include <string>
#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

void APIENTRY debugOutput(GLenum a_source, GLenum a_type, GLuint a_id, GLenum a_severity, GLsizei a_length, const GLchar* a_message, const void* a_userParam)
{
	if (a_id == 131218)	// gl state recompile mismatch 
		return;

	char debSource[32], debType[32], debSev[32];

	switch (a_source)
	{
	case GL_DEBUG_SOURCE_API_ARB:             strcpy_s(debSource, "OpenGL"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   strcpy_s(debSource, "Windows"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: strcpy_s(debSource, "Shader Compiler"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     strcpy_s(debSource, "Third Party"); break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:     strcpy_s(debSource, "Application"); break;
	case GL_DEBUG_SOURCE_OTHER_ARB:           strcpy_s(debSource, "Other"); break;
	default: assert(false);
	}

	switch (a_type)
	{
	case GL_DEBUG_TYPE_ERROR:               strcpy_s(debType, "error"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strcpy_s(debType, "deprecated behavior"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  strcpy_s(debType, "undefined behavior"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         strcpy_s(debType, "portability"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         strcpy_s(debType, "performance"); break;
	case GL_DEBUG_TYPE_OTHER:               strcpy_s(debType, "message"); break;
	case GL_DEBUG_TYPE_MARKER:              strcpy_s(debType, "marker"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          strcpy_s(debType, "push group"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           strcpy_s(debType, "pop group"); break;
	default: assert(false);
	}

	switch (a_severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB:     strcpy_s(debSev, "high"); break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:   strcpy_s(debSev, "medium"); break;
	case GL_DEBUG_SEVERITY_LOW_ARB:      strcpy_s(debSev, "low"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: strcpy_s(debSev, "notification"); break;
	default: assert(false);
	}

	print("%s: %s(%s) %d: %s\n", debSource, debType, debSev, a_id, a_message);
}

END_UNNAMED_NAMESPACE()

void ARBDebugOutput::tryEnable()
{
#if defined(GL_ARB_debug_output)
	if (glewIsExtensionSupported("GL_ARB_debug_output"))
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(debugOutput, NULL);
		glDebugMessageControlARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
		glDebugMessageControlARB(GL_DEBUG_SOURCE_THIRD_PARTY_ARB, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
	}
	else
	{
		print("GL_ARB_debug_output not supported\n");
	}
#endif
}