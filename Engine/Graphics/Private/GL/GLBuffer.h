#pragma once

#include "Graphics/IBuffer.h"

class GLBuffer
{
public:

	 //EBufferType getType() const;

	 void initialize(uint64 sizeBytes, EBufferUsageType usageType);
	 void upload(span<const byte> data);
	 span<byte> map();
	 void unmap();
	 uint64 getSizeBytes() const  { return m_sizeBytes; }

	uint getID() { return m_id; }

private:

	bool m_initialized = false;
	uint64 m_sizeBytes = 0;
	uint m_id = 0;
	uint m_usageFlags = 0;
};