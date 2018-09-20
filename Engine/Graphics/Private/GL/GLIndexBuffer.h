#pragma once

#include "Graphics/IBuffer.h"

class GLIndexBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::INDEX; }

	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) override;
	virtual void upload(span<const byte> data) override;
	virtual void bind() override;

private:

	friend class GLContext;
	GLIndexBuffer() {}
	~GLIndexBuffer() override {}
	GLIndexBuffer(const GLIndexBuffer&) = delete;
};