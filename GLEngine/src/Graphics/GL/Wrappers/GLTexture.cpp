#include "Graphics/GL/Wrappers/GLTexture.h"

#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Utils/getGLTextureFormat.h"

#include <assert.h>
#include <functional>

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &m_textureID);
}

void GLTexture::bind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void GLTexture::unbind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool isMipMapFilter(GLenum filter)
{
	switch (filter)
	{
	case GL_NEAREST_MIPMAP_LINEAR:	return true;
	case GL_NEAREST_MIPMAP_NEAREST: return true;
	case GL_LINEAR_MIPMAP_LINEAR:	return true;
	case GL_LINEAR_MIPMAP_NEAREST:	return true;
	default:						return false;
	}
}

void GLTexture::initialize(const FileHandle& a_file, int a_textureIdx, ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter,
						   ETextureWrap a_textureWrapS, ETextureWrap a_textureWrapT)
{
	Pixmap pixmap;
	pixmap.read(a_file);

	if (!pixmap.exists())
		return;

	m_width         = pixmap.m_width;
	m_height        = pixmap.m_height;
	m_numComponents = pixmap.m_numComponents;

	const GLint internalFormat = getInternalFormatForNumComponents(m_numComponents, pixmap.m_isFloatData);
	const GLint format         = getFormatForNumComponents(m_numComponents);
	const bool generateMipMaps = isMipMapFilter((GLenum) a_minFilter);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum) a_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum) a_magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum) a_textureWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum) a_textureWrapT);
	if (generateMipMaps)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, pixmap.m_isFloatData ? GL_FLOAT : GL_UNSIGNED_BYTE, pixmap.m_data.b);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, pixmap.m_isFloatData ? GL_FLOAT : GL_UNSIGNED_BYTE, pixmap.m_data.b);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}