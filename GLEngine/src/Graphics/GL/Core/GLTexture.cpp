#include "Graphics/GL/Core/GLTexture.h"

#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Utils/getGLTextureFormat.h"

#include <functional>
#include <assert.h>

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

void GLTexture::initialize(const FileHandle& a_file, GLint a_minFilter, GLint a_magFilter,
	GLint a_textureWrapS, GLint a_textureWrapT)
{
	Pixmap pixmap;
	pixmap.read(a_file);

	if (!pixmap.exists())
		return;

	m_width = pixmap.m_width;
	m_height = pixmap.m_height;
	m_numComponents = pixmap.m_numComponents;

	GLint internalFormat = getInternalFormatForNumComponents(m_numComponents, pixmap.m_isFloatData);
	GLint format = getFormatForNumComponents(m_numComponents);

	bool generateMipMaps = isMipMapFilter(a_minFilter);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, a_textureWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, a_textureWrapT);
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