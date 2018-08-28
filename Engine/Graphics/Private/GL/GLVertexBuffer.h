#pragma once

#include "Graphics/IBuffer.h"

class GLVertexBuffer : public IBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::VERTEX; }

private:

	friend class GLContext;
	GLVertexBuffer() {}
	~GLVertexBuffer() override {}
	GLVertexBuffer(const GLVertexBuffer&) = delete;
};