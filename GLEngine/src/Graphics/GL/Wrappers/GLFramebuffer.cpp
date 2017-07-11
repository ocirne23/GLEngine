#include "Graphics/GL/Wrappers/GLFramebuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/Utils/CheckGLError.h"

#include <assert.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

bool GLFramebuffer::s_begun = false;

GLFramebuffer::~GLFramebuffer()
{
	if (m_initialized)
		glDeleteFramebuffers(1, &m_fbo);
}

void GLFramebuffer::initialize(GLTexture::EMultiSampleType a_multiSampleType)
{
	if (m_initialized)
		glDeleteFramebuffers(1, &m_fbo);
	m_multiSampleType = a_multiSampleType;
	m_textures.clear();
	m_drawBuffers.clear();
	glGenFramebuffers(1, &m_fbo);
	m_initialized = true;
}

void GLFramebuffer::addFramebufferTexture(GLTexture& a_texture, EAttachment a_attachment)
{
	assert(m_initialized);
	assert(uint(m_multiSampleType) == uint(a_texture.getMultiSampleType()));

	m_textures.push_back(a_texture.getTextureID());
	m_drawBuffers.push_back(a_attachment);

	CHECK_GL_ERROR();
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GLenum(a_attachment), a_texture.getTextureID(), 0);
	glDrawBuffers(uint(m_drawBuffers.size()), rcast<GLenum*>(&m_drawBuffers[0]));

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERROR();
}

void GLFramebuffer::setDepthbufferTexture(GLTexture& a_texture)
{
	assert(m_initialized);
	assert(uint(m_multiSampleType) == uint(a_texture.getMultiSampleType()));

	m_depthTexture = a_texture.getTextureID();

	CHECK_GL_ERROR()
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, a_texture.getTextureID(), 0);
	
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
}

void GLFramebuffer::bindTexture(uint a_textureIndex, uint a_textureUnit)
{
	assert(m_initialized);
	assert(a_textureIndex < uint(m_textures.size()));
	assert(a_textureIndex >= 0);
	assert(a_textureUnit >= 0);

	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == GLTexture::EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(textureType, m_textures[a_textureIndex]);
}

void GLFramebuffer::bindDepthTexture(uint a_textureUnit)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == GLTexture::EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	//GLenum textureType = GL_TEXTURE_2D;
	glBindTexture(textureType, m_depthTexture);
}

void GLFramebuffer::unbindTexture(uint a_textureIndex, uint a_textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	GLenum textureType = (m_multiSampleType == GLTexture::EMultiSampleType::NONE) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(textureType, 0);
}
