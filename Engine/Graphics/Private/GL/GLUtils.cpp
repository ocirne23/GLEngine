#include "GLUtils.h"

#include "Graphics/IBuffer.h"
#include "Graphics/IFramebuffer.h"
//#include "GLVertexAttributes.h"
#include "GL.h"

BEGIN_NAMESPACE(GLUtils)

uint getGLUsageFlags(const EBufferUsageType& usageType)
{
	switch (usageType)
	{
	case EBufferUsageType::STATIC:  return GL_STATIC_DRAW;
	case EBufferUsageType::DYNAMIC: return GL_DYNAMIC_DRAW;
	case EBufferUsageType::STREAM:  return GL_STREAM_DRAW;
	default: assert(false); return 0;
	}
}

uint getGLAttachment(const ETextureAttachment& attachment)
{
	switch (attachment)
	{
	case ETextureAttachment::DEPTH:   return GL_DEPTH_ATTACHMENT;
	case ETextureAttachment::STENCIL: return GL_STENCIL_ATTACHMENT;
	case ETextureAttachment::COLOR0:  return GL_COLOR_ATTACHMENT0;
	case ETextureAttachment::COLOR1:  return GL_COLOR_ATTACHMENT1;
	case ETextureAttachment::COLOR2:  return GL_COLOR_ATTACHMENT2;
	case ETextureAttachment::COLOR3:  return GL_COLOR_ATTACHMENT3;
	case ETextureAttachment::COLOR4:  return GL_COLOR_ATTACHMENT4;
	case ETextureAttachment::COLOR5:  return GL_COLOR_ATTACHMENT5;
	default: assert(false); return 0;
	}
}

/*
uint getGLVertexAttributeSize(const EVertexAttributeFormat& attribute)
{
	switch (attribute)
	{
	case EVertexAttributeFormat::FLOAT:         return 4;
	case EVertexAttributeFormat::INT:           return 4;
	case EVertexAttributeFormat::UNSIGNED_BYTE: return 1;
	case EVertexAttributeFormat::UNSIGNED_INT:  return 4;
	default: assert(false); return 0;
	}
}

uint getGLVertexAttributeType(const EVertexAttributeFormat& attribute)
{
	switch (attribute)
	{
	case EVertexAttributeFormat::FLOAT:         return GL_FLOAT;
	case EVertexAttributeFormat::INT:           return GL_INT;
	case EVertexAttributeFormat::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	case EVertexAttributeFormat::UNSIGNED_INT:  return GL_UNSIGNED_INT;
	default: assert(false); return 0;
	}
}

*/
END_NAMESPACE(GLUtils)