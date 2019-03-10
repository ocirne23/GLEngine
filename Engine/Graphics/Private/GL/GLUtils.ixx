#include "Core.h"
#include "GL.h"

export module Graphics.GLUtils;

import Graphics.IBuffer;
import Graphics.IFramebuffer;
import Graphics.ITexture;
import std.core;

export BEGIN_NAMESPACE(GLUtils)

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
}*/

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

uint getGLTextureType(const ETextureType& type)
{
	switch (type)
	{
	case ETextureType::TEX1D:        return GL_TEXTURE_1D;
	case ETextureType::TEX2D:        return GL_TEXTURE_2D;
	case ETextureType::TEX3D:        return GL_TEXTURE_3D;
	case ETextureType::TEX1DARRAY:   return GL_TEXTURE_1D_ARRAY;
	case ETextureType::TEX2DARRAY:   return GL_TEXTURE_2D_ARRAY;
	case ETextureType::TEXCUBE:      return GL_TEXTURE_CUBE_MAP;
	case ETextureType::TEXCUBEARRAY: return GL_TEXTURE_CUBE_MAP_ARRAY;
	case ETextureType::TEXBUFFER:    return GL_TEXTURE_BUFFER;
	case ETextureType::TEX2DMS:      return GL_TEXTURE_2D_MULTISAMPLE;
	case ETextureType::TEX2DMSARRAY: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
	default: assert(false); return 0;
	}
}

uint getGLMultisampleType(const EMultisampleType& type)
{
	switch (type)
	{
	case EMultisampleType::NONE:    return 0;
	case EMultisampleType::MSAA_2XFIXEDSAMPLE:
	case EMultisampleType::MSAA_2X: return 2;
	case EMultisampleType::MSAA_4XFIXEDSAMPLE:
	case EMultisampleType::MSAA_4X: return 4;
	case EMultisampleType::MSAA_8XFIXEDSAMPLE:
	case EMultisampleType::MSAA_8X: return 8;
	default: assert(false); return 0;
	}
}

bool getGLMultisampleFixed(const EMultisampleType& type)
{
	switch (type)
	{
	case EMultisampleType::MSAA_2XFIXEDSAMPLE:
	case EMultisampleType::MSAA_4XFIXEDSAMPLE:
	case EMultisampleType::MSAA_8XFIXEDSAMPLE:
		return true;
	default:
		return false;
	}
}

uint getGLTextureFilter(const ETextureMinFilter& filter)
{
	switch (filter)
	{
	case ETextureMinFilter::NEAREST:                return GL_NEAREST;
	case ETextureMinFilter::LINEAR:                 return GL_LINEAR;
	case ETextureMinFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
	case ETextureMinFilter::LINEAR_MIPMAP_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
	case ETextureMinFilter::NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
	case ETextureMinFilter::LINEAR_MIPMAP_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
	default: assert(false); return 0;
	}
}

uint getGLTextureFilter(const ETextureMagFilter& filter)
{
	switch (filter)
	{
	case ETextureMagFilter::NEAREST: return GL_NEAREST;
	case ETextureMagFilter::LINEAR:  return GL_LINEAR;
	default: assert(false); return 0;
	}
}

uint getGLTextureWrap(const ETextureWrap& wrap)
{
	switch (wrap)
	{
	case ETextureWrap::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
	case ETextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
	case ETextureWrap::REPEAT:          return GL_REPEAT;
	default: assert(false); return 0;
	}
}

uint getGLTextureCompare(const ETextureCompareMode& mode)
{
	switch (mode)
	{
	case ETextureCompareMode::NONE:                 return GL_NONE;
	case ETextureCompareMode::COMPARE_R_TO_TEXTURE: return GL_COMPARE_R_TO_TEXTURE;
	default: assert(false); return 0;
	}
}

uint getGLSizedInternalFormat(const ETextureComponentFormat& format, uint numComponents)
{
	assert(numComponents && numComponents <= 4);
	switch (format)
	{
	case ETextureComponentFormat::F8:       return std::array<uint, 4>{ GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 }[numComponents];
	case ETextureComponentFormat::F16:      return std::array<uint, 4>{ GL_R16, GL_RG16, GL_RGB16, GL_RGBA16 }[numComponents];
	case ETextureComponentFormat::F32:      return std::array<uint, 4>{ GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F }[numComponents];
	case ETextureComponentFormat::F8SNORM:  return std::array<uint, 4>{ GL_R8_SNORM, GL_RG8_SNORM, GL_RGB8_SNORM, GL_RGBA8_SNORM }[numComponents];
	case ETextureComponentFormat::F16SNORM: return std::array<uint, 4>{ GL_R16_SNORM, GL_RG16_SNORM, GL_RGB16_SNORM, GL_RGBA16_SNORM }[numComponents];
	case ETextureComponentFormat::I8:       return std::array<uint, 4>{ GL_R8I, GL_RG8I, GL_RGB8I, GL_RGBA8I }[numComponents];
	case ETextureComponentFormat::I16:		return std::array<uint, 4>{ GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I }[numComponents];
	case ETextureComponentFormat::I32:		return std::array<uint, 4>{ GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I }[numComponents];
	case ETextureComponentFormat::UI8:		return std::array<uint, 4>{ GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGBA8UI }[numComponents];
	case ETextureComponentFormat::UI16:		return std::array<uint, 4>{ GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGBA16UI }[numComponents];
	case ETextureComponentFormat::UI32:		return std::array<uint, 4>{ GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI }[numComponents];
	case ETextureComponentFormat::BPTC_RGBA:               assert(numComponents == 4); return GL_COMPRESSED_RGBA_BPTC_UNORM;
	case ETextureComponentFormat::BPTC_SRGBA:              assert(numComponents == 4); return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
	case ETextureComponentFormat::BPTC_RGB_FLOAT:          assert(numComponents == 3); return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
	case ETextureComponentFormat::BPTC_RGB_UNSIGNED_FLOAT: assert(numComponents == 3); return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
	case ETextureComponentFormat::DXT1_RGB:    assert(numComponents == 3); return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	case ETextureComponentFormat::DXT1A_RGBA:  assert(numComponents == 4); return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case ETextureComponentFormat::DXT3_RGBA:   assert(numComponents == 4); return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case ETextureComponentFormat::DXT5_RGBA:   assert(numComponents == 4); return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case ETextureComponentFormat::DXT1_SRGB:   assert(numComponents == 3); return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
	case ETextureComponentFormat::DXT1A_SRGBA: assert(numComponents == 4); return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
	case ETextureComponentFormat::DXT3_SRGBA:  assert(numComponents == 4); return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
	case ETextureComponentFormat::DXT5_SRGBA:  assert(numComponents == 4); return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
	default: assert(false); return 0;
	}
}

bool isCompressed(const ETextureComponentFormat& format)
{
	switch (format)
	{
	case ETextureComponentFormat::BPTC_RGBA:
	case ETextureComponentFormat::BPTC_SRGBA:
	case ETextureComponentFormat::BPTC_RGB_FLOAT:
	case ETextureComponentFormat::BPTC_RGB_UNSIGNED_FLOAT:
	case ETextureComponentFormat::DXT1_RGB:
	case ETextureComponentFormat::DXT1A_RGBA:
	case ETextureComponentFormat::DXT3_RGBA:
	case ETextureComponentFormat::DXT5_RGBA:
	case ETextureComponentFormat::DXT1_SRGB:
	case ETextureComponentFormat::DXT1A_SRGBA:
	case ETextureComponentFormat::DXT3_SRGBA:
	case ETextureComponentFormat::DXT5_SRGBA:
		return true;
	default:
		return false;
	}
}

uint getGLPixelDataFormat(const ETexturePixelDataFormat& format)
{
	switch (format)
	{
	case ETexturePixelDataFormat::UNSIGNED_BYTE:		   return GL_UNSIGNED_BYTE;
	case ETexturePixelDataFormat::BYTE:					   return GL_BYTE;
	case ETexturePixelDataFormat::UNSIGNED_SHORT:		   return GL_UNSIGNED_SHORT;
	case ETexturePixelDataFormat::SHORT:				   return GL_SHORT;
	case ETexturePixelDataFormat::UNSIGNED_INT:			   return GL_UNSIGNED_INT;
	case ETexturePixelDataFormat::INT:					   return GL_INT;
	case ETexturePixelDataFormat::FLOAT:				   return GL_FLOAT;
	case ETexturePixelDataFormat::UNSIGNED_BYTE_3_3_2:	   return GL_UNSIGNED_BYTE_3_3_2;
	case ETexturePixelDataFormat::UNSIGNED_SHORT_5_6_5:	   return GL_UNSIGNED_SHORT_5_6_5;
	case ETexturePixelDataFormat::UNSIGNED_SHORT_4_4_4_4:  return GL_UNSIGNED_SHORT_4_4_4_4;
	case ETexturePixelDataFormat::UNSIGNED_SHORT_5_5_5_1:  return GL_UNSIGNED_SHORT_5_5_5_1;
	case ETexturePixelDataFormat::UNSIGNED_INT_8_8_8_8:	   return GL_UNSIGNED_INT_8_8_8_8;
	case ETexturePixelDataFormat::UNSIGNED_INT_10_10_10_2: return GL_UNSIGNED_INT_10_10_10_2;
	default: assert(false); return 0;
	}
}

uint getGLFormatForNumComponents(uint numComponents)
{
	switch (numComponents)
	{
	case 1: return GL_RED;
	case 2: return GL_RG;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	default: assert(false); return 0;
	}
}

uint getGLFramebufferTarget(const EFramebufferTarget& target)
{
	switch (target)
	{
	case EFramebufferTarget::DRAW:     return GL_DRAW_FRAMEBUFFER;
	case EFramebufferTarget::READ:     return GL_READ_FRAMEBUFFER;
	case EFramebufferTarget::DRAWREAD: return GL_FRAMEBUFFER;
	default: assert(false); return 0;
	}
}

END_NAMESPACE(GLUtils)