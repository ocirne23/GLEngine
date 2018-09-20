#pragma once

#include "Graphics/IBuffer.h"

class GLTextureBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::TEXBUF; }

	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) override;
	virtual void upload(span<const byte> data) override;
	virtual void bind() override;

private:

	friend class GLContext;
	GLTextureBuffer() {}
	~GLTextureBuffer() override {}
	GLTextureBuffer(const GLTextureBuffer&) = delete;
};