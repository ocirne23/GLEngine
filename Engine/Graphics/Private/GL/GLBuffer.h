#pragma once

#include "Graphics/IBuffer.h"

class GLBuffer final : public IBuffer
{
public:

	 GLBuffer(uint64 sizeBytes, EBufferUsageType usageType);
	 virtual ~GLBuffer() override;

	 virtual void       upload(span<const byte> data) override;
	 virtual span<byte> map() override;
	 virtual void       unmap() override;
	 virtual uint64     getSizeBytes() const override { return m_sizeBytes; }

	 uint               getID() { return m_id; }

private:

	uint64 m_sizeBytes = 0;
	uint m_id          = 0;
	uint m_usageFlags  = 0;
};