#include "Graphics/GL/Wrappers/GLFramebuffer.h"

#include "Graphics/GL/GL.h"

#include <assert.h>

bool GLFramebuffer::s_begun = false;

GLFramebuffer::~GLFramebuffer()
{
	if (m_initialized)
	{
		deleteTextures();
		glDeleteFramebuffers(1, &m_fbo);
	}
}

void GLFramebuffer::initialize(EMultiSampleType a_multiSampleType)
{
	assert(!m_initialized);
	m_multiSampleType = a_multiSampleType;
	glGenFramebuffers(1, &m_fbo);
	m_initialized = true;
}

void GLFramebuffer::setDepthbufferTexture(ESizedFormat a_format, uint a_width, uint a_height)
{
	assert(m_initialized);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glGenTextures(1, &m_depthTexture);
	glBindTexture(textureType, m_depthTexture);
	if (m_multiSampleType == EMultiSampleType::NONE)
	{
		glTexStorage2D(textureType, 1, (GLenum) a_format, a_width, a_height);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexStorage2DMultisample(textureType, (GLsizei) m_multiSampleType, (GLenum) a_format, a_width, a_height, GL_TRUE);
	}
	
	begin();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureType, m_depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	end();

	glBindTexture(textureType, 0);
}

void GLFramebuffer::addFramebufferTexture(ESizedFormat a_format, EAttachment a_attachment, uint a_width, uint a_height)
{
	assert(m_initialized);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	uint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(textureType, textureID);
	if (m_multiSampleType == EMultiSampleType::NONE)
	{
		glTexStorage2D(textureType, 1, (GLenum) a_format, a_width, a_height);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexStorage2DMultisample(textureType, (GLsizei) m_multiSampleType, (GLenum) a_format, a_width, a_height, GL_FALSE);
	}

	begin();

	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum) a_attachment, textureType, textureID, 0);

	m_textures.push_back(textureID);
	m_drawBuffers.push_back(a_attachment);

	glDrawBuffers(m_drawBuffers.size(), (GLenum*) &m_drawBuffers[0]);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
	
	end();

	glBindTexture(textureType, 0);
}

void GLFramebuffer::end()
{
	assert(m_initialized);
	assert(m_begun);
	assert(s_begun);
	m_begun = false;
	s_begun = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::begin()
{
	assert(m_initialized);
	assert(!s_begun);
	assert(!m_begun);
	m_begun = true;
	s_begun = true;
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void GLFramebuffer::bindTextures(uint a_fromTextureUnit)
{
	assert(m_initialized);
	for (uint i = 0; i < m_textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i + a_fromTextureUnit);
		GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
		glBindTexture(textureType, m_textures[i]);
	}
}

void GLFramebuffer::bindTexture(uint a_textureIndex, uint a_textureUnit)
{
	assert(m_initialized);
	assert(a_textureIndex < (uint) m_textures.size());
	assert(a_textureIndex >= 0);
	assert(a_textureUnit >= 0);

	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(textureType, m_textures[a_textureIndex]);
}

void GLFramebuffer::bindDepthTexture(uint a_textureUnit)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(textureType, m_depthTexture);
}

void GLFramebuffer::deleteTextures()
{
	assert(m_initialized);
	glDeleteTextures(m_textures.size(), &m_textures[0]);
	m_textures.clear();
	
	if (m_depthTexture)
	{
		glDeleteTextures(1, &m_depthTexture);
		m_depthTexture = 0;
	}
}