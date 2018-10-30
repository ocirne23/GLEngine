#include "GLShader.h"

#include "GL/GL.h"

void GLShader::initialize()
{
	m_id = glCreateProgram();
}

void GLShader::addShaderStage(IShaderStage& stage)
{
	glAttachShader(m_id, 1);
}