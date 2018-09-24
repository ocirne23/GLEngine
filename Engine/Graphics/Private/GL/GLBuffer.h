#pragma once

#include "Graphics/IBuffer.h"

class GLBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override = 0;

	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) override;
	virtual void upload(span<const byte> data) override;
	virtual span<byte> map() override;
	virtual void unmap() override;
	virtual uint64 getSizeBytes() const override { return m_sizeBytes; }

	uint getID() { return m_id; }

private:

	bool m_initialized = false;
	uint64 m_sizeBytes = 0;
	uint m_id = 0;
	uint m_usageFlags = 0;

protected:

	friend class GLContext;
	GLBuffer() {}
	virtual ~GLBuffer() override {}
	GLBuffer(const GLBuffer&) = delete;
};