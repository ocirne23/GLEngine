#include "Graphics\ARBDebugOutput.h"

#ifndef ANDROID
#include "Core.h"

#include <GLEW\glew.h>
#include <string>
#include <assert.h>
#include <SDL\SDL_syswm.h>

void APIENTRY debugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	char debSource[32], debType[32], debSev[32];

	if (source == GL_DEBUG_SOURCE_API_ARB)
		strcpy_s(debSource, "OpenGL");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		strcpy_s(debSource, "Windows");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		strcpy_s(debSource, "Shader Compiler");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		strcpy_s(debSource, "Third Party");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		strcpy_s(debSource, "Application");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		strcpy_s(debSource, "Other");
	else
		assert(0);

	if (type == GL_DEBUG_TYPE_ERROR)
		strcpy_s(debType, "error");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		strcpy_s(debType, "deprecated behavior");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		strcpy_s(debType, "undefined behavior");
	else if (type == GL_DEBUG_TYPE_PORTABILITY)
		strcpy_s(debType, "portability");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE)
		strcpy_s(debType, "performance");
	else if (type == GL_DEBUG_TYPE_OTHER)
		strcpy_s(debType, "message");
	else if (type == GL_DEBUG_TYPE_MARKER)
		strcpy_s(debType, "marker");
	else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy_s(debType, "push group");
	else if (type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy_s(debType, "pop group");
	else
		assert(0);

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		strcpy_s(debSev, "high");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		strcpy_s(debSev, "medium");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		strcpy_s(debSev, "low");
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		strcpy_s(debSev, "notification");
	else
		assert(0);

	if (id == 131218)	//TODO:? gl state recompile mismatch 
		return;

	print("%s: %s(%s) %d: %s\n", debSource, debType, debSev, id, message);
}
#endif // ANDROID

void tryEnableARBDebugOutput()
{
	return;
#if !defined(ANDROID) && defined(GL_ARB_debug_output)
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