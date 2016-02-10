#include "Graphics/GL/Wrappers/GLFramebuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/Utils/CheckGLError.h"

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

void GLFramebuffer::setDepthbufferTexture(ESizedFormat a_format, uint a_width, uint a_height, ETextureMagFilter a_magFilter, ETextureMinFilter a_minFilter)
{
	assert(m_initialized);
	GLenum textureType;
	glGenTextures(1, &m_depthTexture);
	if (m_multiSampleType == EMultiSampleType::NONE)
	{
		textureType = GL_TEXTURE_2D;
		glBindTexture(textureType, m_depthTexture);
		glTexStorage2D(textureType, 1, GLenum(a_format), a_width, a_height);
	}
	else
	{
		textureType = GL_TEXTURE_2D_MULTISAMPLE;
		glBindTexture(textureType, m_depthTexture);
		glTexStorage2DMultisample(textureType, GLsizei(m_multiSampleType), GLenum(a_format), a_width, a_height, GL_TRUE);
	}
	
	// Filter depth texture as nearest because its only used by anti aliasing and ambient occlusion algorithms which all need nearest filtering.
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GLint(a_magFilter));
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GLint(a_minFilter));
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(textureType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::addFramebufferTexture(ESizedFormat a_format, EAttachment a_attachment, uint a_width, uint a_height)
{
	assert(m_initialized);
	CHECK_GL_ERROR();
	
	GLenum textureType;
	uint textureID;
	glGenTextures(1, &textureID);
	if (m_multiSampleType == EMultiSampleType::NONE)
	{
		textureType = GL_TEXTURE_2D;
		glBindTexture(textureType, textureID);
		glTexStorage2D(textureType, 1, GLenum(a_format), a_width, a_height);
	}
	else
	{
		textureType = GL_TEXTURE_2D_MULTISAMPLE;
		glBindTexture(textureType, textureID);
		glTexStorage2DMultisample(textureType, GLsizei(m_multiSampleType), GLenum(a_format), a_width, a_height, GL_TRUE);
	}
	
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GLenum(a_attachment), textureID, 0);

	m_textures.push_back(textureID);
	m_drawBuffers.push_back(a_attachment);

	glDrawBuffers(m_drawBuffers.size(), rcast<GLenum*>(&m_drawBuffers[0]));
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERROR();
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
	glEnable(GL_MULTISAMPLE);
}

void GLFramebuffer::bindTexture(uint a_textureIndex, uint a_textureUnit)
{
	assert(m_initialized);
	assert(a_textureIndex < uint(m_textures.size()));
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

void GLFramebuffer::unbindTexture(uint a_textureIndex, uint a_textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(textureType, 0);
}
