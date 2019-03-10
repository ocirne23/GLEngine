#include "Core.h"
#include "GL.h"

export module Graphics.GLBuffer;

import Graphics.IBuffer;
import Graphics.GLUtils;

export class GLBuffer final : public IBuffer
{
public:

	GLBuffer(uint64 sizeBytes, EBufferUsageType usageType)
	{
		glCreateBuffers(1, &m_id);
		m_usageFlags = GLUtils::getGLUsageFlags(usageType);
		glNamedBufferStorage(m_id, sizeBytes, nullptr, m_usageFlags);
		m_sizeBytes = sizeBytes;
	}

	virtual ~GLBuffer() override
	{
		glDeleteBuffers(1, &m_id);
	}

	virtual void upload(array_view<const byte> data) override
	{
		assert(uint64(data.size_bytes()) <= m_sizeBytes);
		void* mem = glMapNamedBuffer(m_id, GL_MAP_WRITE_BIT);
		std::memcpy(mem, data.data(), std::min(uint64(data.size_bytes()), m_sizeBytes));
		glUnmapNamedBuffer(m_id);
	}

	virtual array_view<byte> map() override
	{
		void* mem = glMapNamedBuffer(m_id, GL_MAP_WRITE_BIT);
		return array_view<byte>(scast<byte*>(mem), m_sizeBytes);
	}

	virtual void unmap() override
	{
		glUnmapNamedBuffer(m_id);
	}

	 virtual uint64 getSizeBytes() const override { return m_sizeBytes; }

	 uint getID() { return m_id; }

private:

	uint64 m_sizeBytes = 0;
	uint m_id          = 0;
	uint m_usageFlags  = 0;
};