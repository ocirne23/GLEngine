#include "Graphics/GL/Wrappers/GLTextureBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"

#include <assert.h>

void GLTextureBuffer::initialize(const GLShader& a_shader, const char* a_samplerName, GLint a_textureIdx, GLenum a_sizedFormat, GLenum a_drawUsage)
{
    assert(a_shader.isBegun());
    assert(!m_initialized);

    m_drawUsage = a_drawUsage;
    m_textureIdx = a_textureIdx;
    m_sizedInternalFormat = a_sizedFormat;
    m_shader = &a_shader;

    m_textureLoc = glGetUniformLocation(a_shader.getID(), a_samplerName);
    assert(m_textureLoc != GL_INVALID_INDEX);

    glUniform1i(m_textureLoc, a_textureIdx);

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
    assert(m_shader->isBegun());
    assert(m_initialized);
    if (a_numBytes)
    {
	glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
	glBufferData(GL_TEXTURE_BUFFER, a_numBytes, a_data, m_drawUsage);
    }
}

void GLTextureBuffer::bind()
{
    assert(m_shader->isBegun());
    glActiveTexture(GL_TEXTURE0 + m_textureIdx);
    glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
}