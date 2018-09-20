#pragma once

#include "Graphics/IBuffer.h"

class GLVertexBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::VERTEX; }

	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) override;
	virtual void upload(span<const byte> data) override;
	virtual void bind() override;

	uint getBufferID() { return m_id; }

private:

	bool m_initialized = false;
	uint m_id = 0;
	uint m_usageFlags = 0;

private:

	friend class GLContext;
	GLVertexBuffer() {}
	~GLVertexBuffer() override {}
	GLVertexBuffer(const GLVertexBuffer&) = delete;
};