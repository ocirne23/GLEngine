#include "Graphics\GL\Core\GLTextureBuffer.h"

#include "Graphics\GL\GL.h"

#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\Core\GLStateBuffer.h"

#include <assert.h>

#ifdef ANDROID

GLenum getFormatForInternalFormat(GLenum internalFormat)
{
	switch (internalFormat)
	{
	case GL_R8: case GL_R8I: case GL_R8UI:
	case GL_R16F: case GL_R16I: case GL_R16UI:
	case GL_R32F: case GL_R32I: case GL_R32UI:
		return GL_RED;
	case GL_RG8: case GL_RG8I: case GL_RG8UI:
	case GL_RG16F: case GL_RG16I: case GL_RG16UI:
	case GL_RG32F: case GL_RG32I: case GL_RG32UI:
		return GL_RG;
	case GL_RGB8: case GL_RGB8I: case GL_RGB8UI:
	case GL_RGB16F: case GL_RGB16I: case GL_RGB16UI:
	case GL_RGB32F: case GL_RGB32I: case GL_RGB32UI:
		return GL_RGB;
	case GL_RGBA8: case GL_RGBA8I: case GL_RGBA8UI:
	case GL_RGBA16F: case GL_RGBA16I: case GL_RGBA16UI:
	case GL_RGBA32F: case GL_RGBA32I: case GL_RGBA32UI:
		return GL_RGBA;
	default:
		assert(false);
		return 4;
	}
}

#endif

void GLTextureBuffer::initialize(const GLShader& shader, const char* samplerName, GLint textureIdx, GLenum format, GLenum drawUsage)
{
	assert(shader.isBegun());
	m_drawUsage = drawUsage;
	m_textureIdx = textureIdx;

	m_textureLoc = glGetUniformLocation(shader.getID(), samplerName);
	assert(m_textureLoc != GL_INVALID_INDEX);
	if (m_textureLoc == GL_INVALID_INDEX)
		return;
	glUniform1i(m_textureLoc, textureIdx);

#ifndef ANDROID
	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);

	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, 0, NULL, m_drawUsage);
	
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, format, m_bufferID);
	
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
#else
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	m_format = getFormatForInternalFormat(format);
	m_internalFormat = format;
#endif
}

GLTextureBuffer::~GLTextureBuffer()
{
#ifndef ANDROID
	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
#endif
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureBuffer::upload(unsigned int numBytes, const void* data)
{
#ifndef ANDROID
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, numBytes, data, m_drawUsage);
#else
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, numBytes, 1, 0, m_internalFormat, m_format, data);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

void GLTextureBuffer::bind()
{
	glActiveTexture(GL_TEXTURE0 + m_textureIdx);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
}

/*


m_lightIndicesTexLoc = glGetUniformLocation(m_shaderID, "u_lightGrid");
glUniform1i(m_lightIndicesTexLoc, 0);

glGenBuffers(1, &m_lightGridBuffer);
glBindBuffer(GL_TEXTURE_BUFFER, m_lightGridBuffer);
glBufferData(GL_TEXTURE_BUFFER, m_lightGrid.size() * sizeof(m_lightGrid[0]), &m_lightGrid[0], GL_STREAM_DRAW);
glGenTextures(1, &m_lightGridTextureBuffer);
glBindTexture(GL_TEXTURE_BUFFER, m_lightGridTextureBuffer);
glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32UI, m_lightGridBuffer);

glBindTexture(GL_TEXTURE_BUFFER, 0);
glBindBuffer(GL_TEXTURE_BUFFER, 0);

*/