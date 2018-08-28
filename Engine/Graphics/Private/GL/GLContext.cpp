#include "GLContext.h"

#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLConstantBuffer.h"
#include "GLTextureBuffer.h"

owner<IBuffer*> GLContext::createBuffer(const EBufferType& type)
{
	switch (type)
	{
	case EBufferType::VERTEX:
		return new GLVertexBuffer();
	case EBufferType::INDEX:
		return new GLIndexBuffer();
	case EBufferType::CONSTANT:
		return new GLConstantBuffer();
	case EBufferType::TEXBUF:
		return new GLTextureBuffer();
	default:
		assert(false);
		break;
	}
	return nullptr;
}

void GLContext::destroyBuffer(owner<IBuffer*> buffer)
{
	switch (buffer->getType())
	{
	case EBufferType::VERTEX:
		delete static_cast<GLVertexBuffer*>(buffer);
		break;
	case EBufferType::INDEX:
		delete static_cast<GLIndexBuffer*>(buffer);
		break;
	case EBufferType::CONSTANT:
		delete static_cast<GLConstantBuffer*>(buffer);
		break;
	case EBufferType::TEXBUF:
		delete static_cast<GLTextureBuffer*>(buffer);
		break;
	default:
		assert(false);
		break;
	}
}

GLContext::GLContext()
{
}

GLContext::~GLContext()
{
}