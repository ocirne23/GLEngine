#pragma once

#include "GLBuffer.h"

class GLIndexBuffer : public GLBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::INDEX; }

private:

	friend class GLContext;
	GLIndexBuffer() {}
	~GLIndexBuffer() override {}
	GLIndexBuffer(const GLIndexBuffer&) = delete;
};