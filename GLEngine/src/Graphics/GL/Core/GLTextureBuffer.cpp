#include "Graphics\GL\Core\GLTextureBuffer.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"

#include <assert.h>

void GLTextureBuffer::initialize(const GLShader& shader, const char* samplerName, GLint textureIdx, GLenum sizedFormat, GLenum drawUsage)
{
	assert(shader.isBegun());
	m_drawUsage = drawUsage;
	m_textureIdx = textureIdx;
	m_sizedInternalFormat = sizedFormat;

	m_textureLoc = glGetUniformLocation(shader.getID(), samplerName);
	assert(m_textureLoc != GL_INVALID_INDEX);
	if (m_textureLoc == GL_INVALID_INDEX)
		return;
	glUniform1i(m_textureLoc, textureIdx);

	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);

	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, 0, NULL, m_drawUsage);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, m_sizedInternalFormat, m_bufferID);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

GLTextureBuffer::~GLTextureBuffer()
{
	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureBuffer::upload(unsigned int numBytes, const void* data)
{
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, numBytes, data, m_drawUsage);
}

void GLTextureBuffer::bind()
{
	glActiveTexture(GL_TEXTURE0 + m_textureIdx);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
}

/*
GLenum getFormatForSizedFormat(GLenum sizedFormat)
{
switch (sizedFormat)
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
return GL_RGBA;
}
}

GLenum getTypeForSizedFormat(GLenum sizedFormat)
{
switch (sizedFormat)
{
case GL_R8: case GL_R8UI:
case GL_RG8: case GL_RG8UI:
case GL_RGB8: case GL_RGB8UI:
case GL_RGBA8: case GL_RGBA8UI:
return GL_UNSIGNED_BYTE;

case GL_R16UI: case GL_RG16UI:
case GL_RGB16UI: case GL_RGBA16UI:
return GL_UNSIGNED_SHORT;

case GL_R32UI: case GL_RG32UI:
case GL_RGB32UI: case GL_RGBA32UI:
return GL_UNSIGNED_INT;

case GL_R8I: case GL_RG8I:
case GL_RGB8I: case GL_RGBA8I:
return GL_BYTE;

case GL_R16I: case GL_RG16I:
case GL_RGB16I: case GL_RGBA16I:
return GL_SHORT;

case GL_R32I: case GL_RG32I:
case GL_RGB32I: case GL_RGBA32I:
return GL_INT;

case GL_R16F: case GL_RG16F:
case GL_RGB16F: case GL_RGBA16F:
return GL_HALF_FLOAT;

case GL_R32F: case GL_RG32F:
case GL_RGB32F: case GL_RGBA32F:
return GL_FLOAT;

default:
assert(false);
return GL_UNSIGNED_BYTE;
}
}

GLenum getInternalFormatForSizedFormat(GLenum sizedFormat)
{
switch (sizedFormat)
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
return GL_RG;
}
}


*/