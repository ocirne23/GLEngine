#include "Graphics/GL/Wrappers/GLTextureBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"

#include <assert.h>

void GLTextureBuffer::initialize(uint a_maxSizeBytes, ESizedFormat a_sizedFormat, EDrawUsage a_drawUsage)
{
	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);

	m_drawUsage = a_drawUsage;
	m_sizedInternalFormat = a_sizedFormat;
	m_maxSizeBytes = a_maxSizeBytes;

	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, a_maxSizeBytes, NULL, (GLenum) m_drawUsage);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
	glTexBuffer(GL_TEXTURE_BUFFER, (GLenum) m_sizedInternalFormat, m_bufferID);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	m_initialized = true;
}

GLTextureBuffer::~GLTextureBuffer()
{
	if (m_bufferID)
		glDeleteBuffers(1, &m_bufferID);
	if (m_textureID)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureBuffer::upload(uint a_numBytes, const void* a_data)
{
	assert(m_initialized);
	assert(a_numBytes <= m_maxSizeBytes);
	a_numBytes = a_numBytes > m_maxSizeBytes ? m_maxSizeBytes : a_numBytes;
	if (a_numBytes)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
		glBufferData(GL_TEXTURE_BUFFER, a_numBytes, a_data, (GLenum) m_drawUsage);
	}
}

void GLTextureBuffer::bind(uint a_index)
{
	assert(m_initialized);	
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
}

byte* GLTextureBuffer::mapBuffer()
{
	assert(m_initialized);
	assert(!m_isMapped);
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	byte* buffer = (byte*) glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	assert(buffer);
	m_isMapped = true;
	return buffer;
}

void GLTextureBuffer::unmapBuffer()
{
	assert(m_initialized);
	assert(m_isMapped);
	m_isMapped = false;
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glUnmapBuffer(GL_TEXTURE_BUFFER);
}
