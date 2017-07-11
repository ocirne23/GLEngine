#pragma once

#include "Core.h"
#include "EASTL/vector.h"
#include "Graphics/GL/Wrappers/GLTexture.h"

class GLFramebuffer
{
public:

	enum class EAttachment
	{
		DEPTH   = 0x8D00, // GL_DEPTH_ATTACHMENT
		STENCIL = 0x8D20, // GL_STENCIL_ATTACHMENT
		COLOR0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
		COLOR1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
		COLOR2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
		COLOR3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
		COLOR4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
		COLOR5  = 0x8CE5  // GL_COLOR_ATTACHMENT5
	};

public:

	GLFramebuffer() {};
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& copy) = delete;

	void initialize(GLTexture::EMultiSampleType multiSampleType = GLTexture::EMultiSampleType::NONE);

	void addFramebufferTexture(GLTexture& texture, EAttachment attachment = EAttachment::COLOR0);
	void setDepthbufferTexture(GLTexture& texture);

	void end();
	void begin();

	void bindDepthTexture(uint textureUnit = 0);
	void bindTexture(uint textureIndex, uint textureUnit);
	void unbindTexture(uint textureIndex, uint textureUnit);

	bool isMultisampleEnabled() const { return m_multiSampleType == GLTexture::EMultiSampleType::NONE; }

private:

	bool m_initialized  = false;
	bool m_begun        = false;
	uint m_fbo          = 0;
	uint m_depthTexture = 0;
	GLTexture::EMultiSampleType m_multiSampleType = GLTexture::EMultiSampleType::NONE;
	eastl::vector<uint> m_textures;
	eastl::vector<EAttachment> m_drawBuffers;

private:

	static bool s_begun;
};