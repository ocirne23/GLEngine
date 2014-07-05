#include "Graphics\GL\Core\GLConstantBuffer.h"

#include "Core.h"

#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\GL.h"

#include <assert.h>

GLConstantBuffer::~GLConstantBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);
}

void GLConstantBuffer::initialize(const GLShader& shader, GLuint bindingPoint, const char* blockName, GLenum drawUsage)
{
	m_drawUsage = drawUsage;
	m_bindingPoint = bindingPoint;

	m_uboIndex = glGetUniformBlockIndex(shader.getID(), blockName);
	if (m_uboIndex == GL_INVALID_INDEX)
	{
		print("Failed to initialize ubo: %s \n", blockName);
		return;
	}

	if (m_ubo)
		glDeleteBuffers(1, &m_ubo);

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);

	glUniformBlockBinding(shader.getID(), m_uboIndex, m_bindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_initialized = true;
}

void GLConstantBuffer::upload(unsigned int numBytes, const void* data)
{
	if (m_initialized)
	{
		print("Uniform buffer upload: %f kb \n", numBytes / 1000.0f);

		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
		glBufferData(GL_UNIFORM_BUFFER, numBytes, data, m_drawUsage);
	}
}

void GLConstantBuffer::bind()
{
	if (m_initialized)
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
}