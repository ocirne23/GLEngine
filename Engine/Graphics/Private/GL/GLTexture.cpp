#include "GLTexture.h"

#include "GL.h"
#include "GLUtils.h"

GLTexture::GLTexture(ETextureType type)
	: m_type(type)
	, m_settings()
{
	glCreateTextures(GLUtils::getGLTextureType(type), 1 , &m_id);
}

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &m_id);
}

void GLTexture::setOptions(const TextureSettings& settings)
{
	m_settings = settings;
	setupTextureStorage(settings.size);

	const TextureFilterSettings& filter = settings.filter;
	glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GLUtils::getGLTextureFilter(filter.minFilter));
	glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GLUtils::getGLTextureFilter(filter.magFilter));
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GLUtils::getGLTextureWrap(filter.sWrap));
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GLUtils::getGLTextureWrap(filter.tWrap));
	glTextureParameteri(m_id, GL_TEXTURE_BASE_LEVEL, 0);
	glTextureParameteri(m_id, GL_TEXTURE_MAX_LEVEL, settings.size.mipLevels);
	if (filter.compare != ETextureCompareMode::NONE)
	{
		glTextureParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GLUtils::getGLTextureCompare(filter.compare));
		glTextureParameteri(m_id, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
}

void GLTexture::setupTextureStorage(const TextureSizeSettings& size)
{
	const uint internalFormat = GLUtils::getGLSizedInternalFormat(size.format, size.numComponents);
	switch (m_type)
	{
	case ETextureType::TEX1D:        return glTextureStorage1D(m_id, size.mipLevels, internalFormat, size.width);
	case ETextureType::TEX2D:        return glTextureStorage2D(m_id, size.mipLevels, internalFormat, size.width, size.height);
	case ETextureType::TEX3D:        return glTextureStorage3D(m_id, size.mipLevels, internalFormat, size.width, size.height, size.depth);
	case ETextureType::TEX1DARRAY:   return glTextureStorage2D(m_id, size.mipLevels, internalFormat, size.width, size.depth);
	case ETextureType::TEX2DARRAY:   return glTextureStorage3D(m_id, size.mipLevels, internalFormat, size.width, size.height, size.depth);
	case ETextureType::TEXCUBE:      return glTextureStorage2D(m_id, size.mipLevels, internalFormat, size.width, size.height);
	case ETextureType::TEXCUBEARRAY: return glTextureStorage3D(m_id, size.mipLevels, internalFormat, size.width, size.height, size.depth);
	case ETextureType::TEXBUFFER:    return assert(false); //return glTextureBuffer(m_id, internalFormat, settings.textureBuffer);			size.
	case ETextureType::TEX2DMS:      return glTextureStorage2DMultisample(m_id, GLUtils::getGLMultisampleType(size.multisample), internalFormat, size.width, size.height, GLUtils::getGLMultisampleFixed(size.multisample));
	case ETextureType::TEX2DMSARRAY: return glTextureStorage3DMultisample(m_id, GLUtils::getGLMultisampleType(size.multisample), internalFormat, size.width, size.height, size.depth, GLUtils::getGLMultisampleFixed(size.multisample));
	default: assert(false);
	}
}

void GLTexture::loadTexture(void* data, ETexturePixelDataFormat format)
{
	const TextureSizeSettings& size = m_settings.size;
	assert(size.width);

	const uint pixelFormat = GLUtils::getGLPixelDataFormat(format);
	const uint componentFormat = GLUtils::getGLFormatForNumComponents(size.numComponents);
	switch (m_type)
	{
	case ETextureType::TEX1D:        return glTextureSubImage1D(m_id, 0, 0,       size.width,                          componentFormat, pixelFormat, data);
	case ETextureType::TEX2D:        return glTextureSubImage2D(m_id, 0, 0, 0,    size.width, size.height,             componentFormat, pixelFormat, data);
	case ETextureType::TEX3D:        return glTextureSubImage3D(m_id, 0, 0, 0, 0, size.width, size.height, size.depth, componentFormat, pixelFormat, data);
	case ETextureType::TEX1DARRAY:   return glTextureSubImage2D(m_id, 0, 0, 0,    size.width, size.depth,              componentFormat, pixelFormat, data);
	case ETextureType::TEX2DARRAY:   return glTextureSubImage3D(m_id, 0, 0, 0, 0, size.width, size.height, size.depth, componentFormat, pixelFormat, data);
	case ETextureType::TEXCUBE:
	{
		for (uint face = 0; face < 6; ++face)
			glTextureSubImage3D(m_id, 0, 0, 0, face, size.width, size.height, 1, componentFormat, pixelFormat, data);
		return;
	}
	case ETextureType::TEXCUBEARRAY:
	{
		for (uint face = 0; face < 6; ++face)
			glTextureSubImage3D(m_id, 0, 0, 0, face, size.width, size.height, size.depth, componentFormat, pixelFormat, data);
		return;
	}
	case ETextureType::TEXBUFFER:    return assert(false);
	case ETextureType::TEX2DMS:      return glTextureSubImage2D(m_id, 0, 0, 0,    size.width, size.height,             componentFormat, pixelFormat, data);
	case ETextureType::TEX2DMSARRAY: return glTextureSubImage3D(m_id, 0, 0, 0, 0, size.width, size.height, size.depth, componentFormat, pixelFormat, data);
	default: assert(false);
	}
}