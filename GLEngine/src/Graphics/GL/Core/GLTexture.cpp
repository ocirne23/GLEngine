#include "Graphics\GL\Core\GLTexture.h"

#include "Graphics\Pixmap.h"
#include "Graphics\GL\GL.h"

#include <assert.h>

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &m_textureID);
}

void GLTexture::bind(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void GLTexture::unbind(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool isMipMapFilter(GLenum filter)
{
	switch (filter)
	{
	case GL_NEAREST_MIPMAP_LINEAR: return true;
	case GL_NEAREST_MIPMAP_NEAREST: return true;
	case GL_LINEAR_MIPMAP_LINEAR: return true;
	case GL_LINEAR_MIPMAP_NEAREST: return true;
	default: return false;
	}
}

void GLTexture::initialize(const char* fileName, GLint minFilter, GLint magFilter,
	GLint textureWrapS, GLint textureWrapT)
{
	Pixmap pixmap;
	pixmap.readRaw(fileName);

	if (!pixmap.exists())
		return;

	m_width = pixmap.m_width;
	m_height = pixmap.m_height;
	m_numComponents = pixmap.m_numComponents;

	GLint internalFormat;
	switch (pixmap.m_numComponents)
	{
	case 1: internalFormat = GL_RED;
		break;
	case 2: internalFormat = GL_RG;
		break;
	case 3: internalFormat = GL_RGB;
		break;
	case 4: internalFormat = GL_RGBA;
		break;
	default: internalFormat = -1;
		break;
	}
	bool generateMipMaps = isMipMapFilter(minFilter);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapT);
	if (generateMipMaps)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, internalFormat, GL_UNSIGNED_BYTE, pixmap.m_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, internalFormat, GL_UNSIGNED_BYTE, pixmap.m_data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}