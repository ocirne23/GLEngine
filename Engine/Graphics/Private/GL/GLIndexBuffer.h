#pragma once

#include "Graphics/IBuffer.h"

class GLIndexBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::INDEX; }

private:

	friend class GLContext;
	GLIndexBuffer() {}
	~GLIndexBuffer() override {}
	GLIndexBuffer(const GLIndexBuffer&) = delete;
};