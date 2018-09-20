#pragma once

#include "Graphics/IBuffer.h"

class GLConstantBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::CONSTANT; }
	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) override;
	virtual void upload(span<const byte> data) override;
	virtual void bind() override;

private:

	friend class GLContext;
	GLConstantBuffer() {}
	~GLConstantBuffer() override {}
	GLConstantBuffer(const GLConstantBuffer&) = delete;
};