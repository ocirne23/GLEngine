#pragma once

#include "Graphics/IBuffer.h"

class GLTextureBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::TEXBUF; }

private:

	friend class GLContext;
	GLTextureBuffer() {}
	~GLTextureBuffer() override {}
	GLTextureBuffer(const GLTextureBuffer&) = delete;
};