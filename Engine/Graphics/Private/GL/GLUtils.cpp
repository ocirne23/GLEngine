#include "GLUtils.h"

#include "Graphics/IBuffer.h"
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

END_NAMESPACE(GLUtils)