#include "Graphics/GL/Wrappers/GLConstantBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/Utils/CheckGLError.h"

#include <assert.h>

void GLConstantBuffer::initialize(const Config& a_config)
{
	initialize(a_config.bindingPoint, a_config.name.c_str(), a_config.drawUsage, a_config.dataSize);
}

void GLConstantBuffer::initialize(uint a_bindingPoint, const char* a_blockName, EDrawUsage a_drawUsage, uint dataSize)
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);

	m_drawUsage = a_drawUsage;
	m_bindingPoint = a_bindingPoint;

	CHECK_GL_ERROR();
	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
	glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, scast<GLenum>(a_drawUsage)); // Reserve memory with glBufferData
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	CHECK_GL_ERROR();

	m_initialized = true;
}

GLConstantBuffer::~GLConstantBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_ubo);
}

GLConstantBuffer::GLConstantBuffer(const GLConstantBuffer& copy)
{
	assert(!m_initialized);
}

void GLConstantBuffer::upload(uint a_numBytes, const void* a_data, uint a_offset)
{
	assert(m_initialized);
	if (a_numBytes)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, a_offset, a_numBytes, a_data);
	}
}

void GLConstantBuffer::bind()
{
	assert(m_initialized);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_ubo);
}

byte* GLConstantBuffer::mapBuffer()
{
	assert(m_initialized);
	assert(!m_isMapped);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	byte* buffer = scast<byte*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
	assert(buffer);
	m_isMapped = true;
	return buffer;
}

void GLConstantBuffer::unmapBuffer()
{
	assert(m_initialized);
	assert(m_isMapped);
	m_isMapped = false;
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

