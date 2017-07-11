#include "Graphics/Utils/TextureFormatUtils.h"
#include "Graphics/GL/GL.h"
#include <assert.h>

GLint TextureFormatUtils::getInternalFormatForNumComponents(uint a_numComponents, bool a_isFloatTexture)
{
	if (a_isFloatTexture)
	{
		switch (a_numComponents)
		{
		case 1: return GL_R32F;
		case 2: return GL_RG32F;
		case 3: return GL_RGB32F;
		case 4: return GL_RGBA32F;
		default:
			assert(false);
			return GL_RGBA32F;
		}
	}
	else
	{
		switch (a_numComponents)
		{
		case 1: return GL_R8;
		case 2: return GL_RG8;
		case 3: return GL_RGB8;
		case 4: return GL_RGBA8;
		default:
			assert(false);
			return GL_RGBA8;
		}
	}
}

GLenum TextureFormatUtils::getFormatForNumComponents(uint a_numComponents)
{
	switch (a_numComponents)
	{
	case 1: return GL_RED;
	case 2: return GL_RG;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	default:
		assert(false);
		return GL_RGBA;
	}
}

uint TextureFormatUtils::getNumComponentsForFormat(GLenum a_format)
{
	switch (a_format)
	{
	case GL_R8:
	case GL_R16:
	case GL_R16F:
	case GL_R32F:
	case GL_R8I:
	case GL_R16I:
	case GL_R32I:
	case GL_R8UI:
	case GL_R16UI:
	case GL_R32UI:
		return 1;
	case GL_RG8:
	case GL_RG16:
	case GL_RG16F:
	case GL_RG32F:
	case GL_RG8I:
	case GL_RG16I:
	case GL_RG32I:
	case GL_RG8UI:
	case GL_RG16UI:
	case GL_RG32UI:
		return 2;
	case GL_RGB8:
	case GL_RGB16:
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_RGB8I:
	case GL_RGB16I:
	case GL_RGB32I:
	case GL_RGB8UI:
	case GL_RGB16UI:
	case GL_RGB32UI:
		return 3;
	case GL_RGBA8:
	case GL_RGBA16:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_RGBA8I:
	case GL_RGBA16I:
	case GL_RGBA32I:
	case GL_RGBA8UI:
	case GL_RGBA16UI:
	case GL_RGBA32UI:
		return 4;
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
		return 1;
	default:
		assert(false);
		return 4;
	}
}
