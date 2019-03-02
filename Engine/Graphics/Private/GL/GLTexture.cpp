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

void GLTexture::setOptions(const ITextureSettings& settings)
{
	m_settings = settings;
	setupTextureStorage(settings);

	glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GLUtils::getGLTextureFilter(settings.minFilter));
	glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GLUtils::getGLTextureFilter(settings.magFilter));
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GLUtils::getGLTextureWrap(settings.sWrap));
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GLUtils::getGLTextureWrap(settings.tWrap));
	glTextureParameteri(m_id, GL_TEXTURE_BASE_LEVEL, 0);
	glTextureParameteri(m_id, GL_TEXTURE_MAX_LEVEL, settings.mipLevels);
	if (settings.compare != ETextureCompareMode::NONE)
	{
		glTextureParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GLUtils::getGLTextureCompare(settings.compare));
		glTextureParameteri(m_id, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
}

void GLTexture::setupTextureStorage(const ITextureSettings& settings)
{
	const uint internalFormat = GLUtils::getGLSizedInternalFormat(settings.format, settings.numComponents);
	switch (m_type)
	{
	case ETextureType::TEX1D:        return glTextureStorage1D(m_id, settings.mipLevels, internalFormat, settings.width);
	case ETextureType::TEX2D:        return glTextureStorage2D(m_id, settings.mipLevels, internalFormat, settings.width, settings.height);
	case ETextureType::TEX3D:        return glTextureStorage3D(m_id, settings.mipLevels, internalFormat, settings.width, settings.height, settings.depth);
	case ETextureType::TEX1DARRAY:   return glTextureStorage2D(m_id, settings.mipLevels, internalFormat, settings.width, settings.depth);
	case ETextureType::TEX2DARRAY:   return glTextureStorage3D(m_id, settings.mipLevels, internalFormat, settings.width, settings.height, settings.depth);
	case ETextureType::TEXCUBE:      return glTextureStorage2D(m_id, settings.mipLevels, internalFormat, settings.width, settings.height);
	case ETextureType::TEXCUBEARRAY: return glTextureStorage3D(m_id, settings.mipLevels, internalFormat, settings.width, settings.height, settings.depth);
	case ETextureType::TEXBUFFER:    return glTextureBuffer(m_id, internalFormat, settings.textureBuffer);
	case ETextureType::TEX2DMS:      return glTextureStorage2DMultisample(m_id, GLUtils::getGLMultisampleType(settings.multisample), internalFormat, settings.width, settings.height, GLUtils::getGLMultisampleFixed(settings.multisample));
	case ETextureType::TEX2DMSARRAY: return glTextureStorage3DMultisample(m_id, GLUtils::getGLMultisampleType(settings.multisample), internalFormat, settings.width, settings.height, settings.depth, GLUtils::getGLMultisampleFixed(settings.multisample));
	default: assert(false);
	}
}