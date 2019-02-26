#include "GLBuffer.h"

#include "GL.h"
#include "GLUtils.h"

#include <EASTL/algorithm.h>

GLBuffer::GLBuffer(uint64 sizeBytes, EBufferUsageType usageType)
{
	glCreateBuffers(1, &m_id);
	m_usageFlags = GLUtils::getGLUsageFlags(usageType);
	glNamedBufferStorage(m_id, sizeBytes, nullptr, m_usageFlags);
	m_sizeBytes = sizeBytes;
}

GLBuffer::~GLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void GLBuffer::upload(span<const byte> data)
{
	assert(uint64(data.size_bytes()) <= m_sizeBytes);
	void* mem = glMapNamedBuffer(m_id, GL_MAP_WRITE_BIT);
	std::memcpy(mem, data.data(), eastl::min(uint64(data.size_bytes()), m_sizeBytes));
	glUnmapNamedBuffer(m_id);
}

span<byte> GLBuffer::map()
{
	void* mem = glMapNamedBuffer(m_id, GL_MAP_WRITE_BIT);
	return make_span(scast<byte*>(mem), m_sizeBytes);
}

void GLBuffer::unmap()
{
	glUnmapNamedBuffer(m_id);
}