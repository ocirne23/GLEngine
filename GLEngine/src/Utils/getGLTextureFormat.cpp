#pragma once

#include "Utils\getGLTextureFormat.h"
#include "Graphics\GL\GL.h"
#include <assert.h>

GLint getInternalFormatForNumComponents(uint numComponents, bool isFloatTexture)
{
	if (isFloatTexture)
	{
		switch (numComponents)
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
		switch (numComponents)
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

GLenum getFormatForNumComponents(uint numComponents)
{
	switch (numComponents)
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