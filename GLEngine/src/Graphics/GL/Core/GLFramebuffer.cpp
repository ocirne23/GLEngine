#include "Graphics/GL/Core/GLFramebuffer.h"

#include "Graphics/GL/GL.h"

#include <assert.h>

bool GLFramebuffer::s_begun = false;

GLFramebuffer::GLFramebuffer() : m_depthTexture(0)
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFramebuffer::~GLFramebuffer()
{
	deleteTextures();
	glDeleteFramebuffers(1, &m_fbo);
}

void GLFramebuffer::setDepthbufferTexture(GLenum a_format, uint a_width, uint a_height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, a_format, a_width, a_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::addFramebufferTexture(GLenum a_format, GLenum a_attachment, uint a_width, uint a_height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, a_format, a_width, a_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, a_attachment, GL_TEXTURE_2D, textureID, 0);
	m_textures.push_back(textureID);

	m_drawBuffers.push_back(a_attachment);
	glDrawBuffers(m_drawBuffers.size(), &m_drawBuffers[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::end()
{
	assert(m_begun);
	assert(s_begun);
	m_begun = false;
	s_begun = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::begin()
{
	assert(!s_begun);
	assert(!m_begun);
	m_begun = true;
	s_begun = true;
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void GLFramebuffer::bindTextures(uint a_fromTextureUnit)
{
	for (int i = 0; i < m_textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i + a_fromTextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
	}
}

void GLFramebuffer::bindTexture(uint a_textureIndex, uint a_textureUnit)
{
	assert(a_textureIndex < (uint) m_textures.size());
	assert(a_textureIndex >= 0);
	assert(a_textureUnit >= 0);

	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textures[a_textureIndex]);
}

void GLFramebuffer::bindDepthTexture(uint a_textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + a_textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
}

void GLFramebuffer::deleteTextures()
{
	if (m_textures.size() > 0)
	{
		glDeleteTextures(m_textures.size(), &m_textures[0]);
	}
	m_textures.clear();

	if (m_depthTexture)
	{
		glDeleteTextures(1, &m_depthTexture);
		m_depthTexture = 0;
	}
}
/*

const vec2 poisson16[] = vec2[](vec2( -0.94201624,  -0.39906216 ),
vec2(  0.94558609,  -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2(  0.34495938,   0.29387760 ),
vec2( -0.91588581,   0.45771432 ),
vec2( -0.81544232,  -0.87912464 ),
vec2( -0.38277543,   0.27676845 ),
vec2(  0.97484398,   0.75648379 ),
vec2(  0.44323325,  -0.97511554 ),
vec2(  0.53742981,  -0.47373420 ),
vec2( -0.26496911,  -0.41893023 ),
vec2(  0.79197514,   0.19090188 ),
vec2( -0.24188840,   0.99706507 ),
vec2( -0.81409955,   0.91437590 ),
vec2(  0.19984126,   0.78641367 ),
vec2(  0.14383161,  -0.14100790 ));

*/