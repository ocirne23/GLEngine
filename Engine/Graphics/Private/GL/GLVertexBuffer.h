#pragma once

#include "GLBuffer.h"

class GLVertexBuffer : public GLBuffer
{
public:

	virtual EBufferType getType() const override { return EBufferType::VERTEX; }

private:

	friend class GLContext;
	GLVertexBuffer() {}
	~GLVertexBuffer() override {}
	GLVertexBuffer(const GLVertexBuffer&) = delete;
};