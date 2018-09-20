#include "GLVertexBuffer.h"

#include "GL.h"
#include "GLUtils.h"

void GLVertexBuffer::initialize(uint64 sizeBytes, EBufferUsageType usageType)
{
	if (m_initialized)
	{
		glDeleteBuffers(1, &m_id);
	}
	glCreateBuffers(1, &m_id);
	m_usageFlags = GLUtils::getGLUsageFlags(usageType);
	m_initialized = true;
}

void GLVertexBuffer::upload(span<const byte> data)
{
	glNamedBufferData(m_id, data.size_bytes(), data.data(), m_usageFlags);
}

void GLVertexBuffer::bind()
{
}
