#include "Graphics/IBuffer.h"

#include "GL/GLBuffer.h"

IBuffer::IBuffer() : m_impl(make_pimpl<GLBuffer>())
{
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
