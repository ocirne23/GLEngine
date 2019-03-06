#include "GLFramebuffer.h"

#include "GL.h"
#include "GLUtils.h"
#include "GLTexture.h"

GLFramebuffer::GLFramebuffer()
{
	glGenFramebuffers(1, &m_id);
}

GLFramebuffer::~GLFramebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void GLFramebuffer::attachTexture(const ITexture& tex, ETextureAttachment attachment)
{
	const GLTexture& texture = static_cast<const GLTexture&>(tex);
	const uint glAttachment = GLUtils::getGLAttachment(attachment);
	glNamedFramebufferTexture(m_id, glAttachment, texture.getID(), 0);
	const auto it = eastl::find(m_drawBuffers.begin(), m_drawBuffers.end(), glAttachment);
	if (it != m_drawBuffers.end())
		m_drawBuffers.push_back(glAttachment);
	glNamedFramebufferDrawBuffers(m_id, (GLsizei)m_drawBuffers.size(), m_drawBuffers.data());
}

void GLFramebuffer::bind(EFramebufferTarget target)
{
	//GLUtils::getGLFramebufferTarget(target)
	//glBindFramebuffer(GLUtils::getGLFramebufferTarget(target), m_id);
}
