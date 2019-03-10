#include "Core.h"
#include "GL.h"

export module Graphics.GLFramebuffer;
import Graphics.IFramebuffer;
import Graphics.GLTexture;
import Graphics.ITexture;
import std.core;

export class GLFramebuffer : public IFramebuffer
{
public:

	GLFramebuffer()
	{
		glGenFramebuffers(1, &m_id);
	}

	~GLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_id);
	}

	virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment)
	{

	}
	/*
	virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment) override
	{
		const GLTexture& texture = static_cast<const GLTexture&>(tex);
		const uint glAttachment = GLUtils::getGLAttachment(attachment);
		glNamedFramebufferTexture(m_id, glAttachment, texture.getID(), 0);
		const auto it = std::find(m_drawBuffers.begin(), m_drawBuffers.end(), glAttachment);
		if (it != m_drawBuffers.end())
			m_drawBuffers.push_back(glAttachment);
		glNamedFramebufferDrawBuffers(m_id, (GLsizei)m_drawBuffers.size(), m_drawBuffers.data());
	}*/

	virtual void bind(EFramebufferTarget target) override
	{
		//GLUtils::getGLFramebufferTarget(target)
		//glBindFramebuffer(GLUtils::getGLFramebufferTarget(target), m_id);
	}

	

private:

	uint m_id = 0;
	EFramebufferTarget m_target = EFramebufferTarget::DRAWREAD;
	std::vector<uint> m_drawBuffers;
};