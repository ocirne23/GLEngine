#include "Graphics/GL/Wrappers/GLConstantBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"

#include <assert.h>

GLConstantBuffer::~GLConstantBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);
}

void GLConstantBuffer::initialize(const GLShader** a_shaders, uint a_numShaders, uint a_bindingPoint, const char* a_blockName, EDrawUsage a_drawUsage)
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);

	m_drawUsage = a_drawUsage;
	m_bindingPoint = a_bindingPoint;

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	for (uint i = 0; i < a_numShaders; ++i)
	{
		const GLShader* shader = a_shaders[i];

		GLuint uboIndex = glGetUniformBlockIndex(shader->getID(), a_blockName);
		if (uboIndex == GL_INVALID_INDEX)
		{
			print("Failed to initialize ubo: %s \n", a_blockName);
			glDeleteBuffers(1, &m_ubo);
			return;
		}
		glUniformBlockBinding(shader->getID(), uboIndex, m_bindingPoint);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_initialized = true;
}

void GLConstantBuffer::initialize(const GLShader& a_shader, uint a_bindingPoint, const char* a_blockName, EDrawUsage a_drawUsage)
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);

	m_drawUsage = a_drawUsage;
	m_bindingPoint = a_bindingPoint;

	GLuint uboIndex = glGetUniformBlockIndex(a_shader.getID(), a_blockName);
	if (uboIndex == GL_INVALID_INDEX)
	{
		print("Failed to initialize ubo: %s \n", a_blockName);
		return;
	}

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);

	glUniformBlockBinding(a_shader.getID(), uboIndex, m_bindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_initialized = true;
}

void GLConstantBuffer::upload(uint a_numBytes, const void* a_data)
{
	assert(m_initialized);
	if (a_numBytes)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
		glBufferData(GL_UNIFORM_BUFFER, a_numBytes, a_data, (GLenum) m_drawUsage);
	}
}

void GLConstantBuffer::bind()
{
	assert(m_initialized);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
}