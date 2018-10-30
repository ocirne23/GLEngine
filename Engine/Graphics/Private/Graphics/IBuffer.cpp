#include "Graphics/IBuffer.h"

#include "GL/GLBuffer.h"

IBuffer::IBuffer() : m_impl(new GLBuffer())
{
}

IBuffer::~IBuffer()
{
	SAFE_DELETE(m_impl);
}

void IBuffer::initialize(uint64 sizeBytes, EBufferUsageType usageType)
{
	m_impl->initialize(sizeBytes, usageType);
}

void IBuffer::upload(span<const byte> data)
{
	m_impl->upload(data);
}

span<byte> IBuffer::map()
{
	return m_impl->map();
}

void IBuffer::unmap()
{
	m_impl->unmap();
}

uint64 IBuffer::getSizeBytes() const
{
	return m_impl->getSizeBytes();
}
