#pragma once

#include "Graphics/IBuffer.h"

class GLConstantBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::CONSTANT; }

private:

	friend class GLContext;
	GLConstantBuffer() {}
	~GLConstantBuffer() override {}
	GLConstantBuffer(const GLConstantBuffer&) = delete;
};