#include "Graphics/tryEnableARBDebugOutput.h"

#include "Core.h"

#include <GLEW/glew.h>
#include <SDL/SDL_syswm.h>

#include <string>
#include <assert.h>

void APIENTRY debugOutput(GLenum a_source, GLenum a_type, GLuint a_id, GLenum a_severity, GLsizei a_length, const GLchar* a_message, const void* a_userParam)
{
	char debSource[32], debType[32], debSev[32];

	if (a_source == GL_DEBUG_SOURCE_API_ARB)
		strcpy_s(debSource, "OpenGL");
	else if (a_source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		strcpy_s(debSource, "Windows");
	else if (a_source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		strcpy_s(debSource, "Shader Compiler");
	else if (a_source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		strcpy_s(debSource, "Third Party");
	else if (a_source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		strcpy_s(debSource, "Application");
	else if (a_source == GL_DEBUG_SOURCE_OTHER_ARB)
		strcpy_s(debSource, "Other");
	else
		assert(0);

	if (a_type == GL_DEBUG_TYPE_ERROR)
		strcpy_s(debType, "error");
	else if (a_type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		strcpy_s(debType, "deprecated behavior");
	else if (a_type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		strcpy_s(debType, "undefined behavior");
	else if (a_type == GL_DEBUG_TYPE_PORTABILITY)
		strcpy_s(debType, "portability");
	else if (a_type == GL_DEBUG_TYPE_PERFORMANCE)
		strcpy_s(debType, "performance");
	else if (a_type == GL_DEBUG_TYPE_OTHER)
		strcpy_s(debType, "message");
	else if (a_type == GL_DEBUG_TYPE_MARKER)
		strcpy_s(debType, "marker");
	else if (a_type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy_s(debType, "push group");
	else if (a_type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy_s(debType, "pop group");
	else
		assert(0);

	if (a_severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		strcpy_s(debSev, "high");
	else if (a_severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		strcpy_s(debSev, "medium");
	else if (a_severity == GL_DEBUG_SEVERITY_LOW_ARB)
		strcpy_s(debSev, "low");
	else if (a_severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		strcpy_s(debSev, "notification");
	else
		assert(0);

	if (a_id == 131218)	//TODO:? gl state recompile mismatch 
		return;

	print("%s: %s(%s) %d: %s\n", debSource, debType, debSev, a_id, a_message);
}

void tryEnableARBDebugOutput()
{
#if defined(GL_ARB_debug_output)
	if (glewIsExtensionSupported("GL_ARB_debug_output"))
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(debugOutput, NULL);
		glDebugMessageControlARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DONT_CARE,
			GL_DONT_CARE, 0, NULL, true);
		glDebugMessageControlARB(GL_DEBUG_SOURCE_THIRD_PARTY_ARB, GL_DONT_CARE,
			GL_DONT_CARE, 0, NULL, true);
	}
	else
	{
		print("GL_ARB_debug_output not supported\n");
	}
#endif
}