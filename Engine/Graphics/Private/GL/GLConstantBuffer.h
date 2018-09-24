#pragma once

#include "GLBuffer.h"

class GLConstantBuffer : public GLBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::CONSTANT; }

private:

	friend class GLContext;
	GLConstantBuffer() {}
	~GLConstantBuffer() override {}
	GLConstantBuffer(const GLConstantBuffer&) = delete;
};