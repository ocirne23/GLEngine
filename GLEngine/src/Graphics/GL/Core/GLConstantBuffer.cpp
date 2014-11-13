#include "Graphics/GL/Core/GLConstantBuffer.h"

#include "Core.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Core/GLShader.h"

#include <assert.h>

GLConstantBuffer::~GLConstantBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);
}

void GLConstantBuffer::initialize(const GLShader& a_shader, GLuint a_bindingPoint, const char* a_blockName, GLenum a_drawUsage)
{
	m_drawUsage = a_drawUsage;
	m_bindingPoint = a_bindingPoint;

	m_uboIndex = glGetUniformBlockIndex(a_shader.getID(), a_blockName);
	if (m_uboIndex == GL_INVALID_INDEX)
	{
		print("Failed to initialize ubo: %s \n", a_blockName);
		return;
	}

	if (m_ubo)
		glDeleteBuffers(1, &m_ubo);

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);

	glUniformBlockBinding(a_shader.getID(), m_uboIndex, m_bindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_initialized = true;
}

void GLConstantBuffer::upload(uint a_numBytes, const void* a_data)
{
	if (m_initialized)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
		glBufferData(GL_UNIFORM_BUFFER, a_numBytes, a_data, m_drawUsage);
	}
}

void GLConstantBuffer::bind()
{
	if (m_initialized)
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
}