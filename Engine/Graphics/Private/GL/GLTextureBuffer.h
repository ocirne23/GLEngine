#pragma once

#include "GLBuffer.h"

class GLTextureBuffer : public GLBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::TEXBUF; }

private:

	friend class GLContext;
	GLTextureBuffer() {}
	~GLTextureBuffer() override {}
	GLTextureBuffer(const GLTextureBuffer&) = delete;
};