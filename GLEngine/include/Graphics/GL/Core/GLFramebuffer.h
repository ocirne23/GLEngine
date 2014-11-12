#pragma once

#include "Core.h"
#include "Graphics\GL\GLTypes.h"

#include "rde\vector.h"

class GLFramebuffer
{
public:
	GLFramebuffer();
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& copy) = delete;

	void addFramebufferTexture(GLenum format, GLenum attachment, uint width, uint height);
	void setDepthbufferTexture(GLenum format, uint width, uint height);

	void end();
	void begin();

	void bindTextures(uint fromTextureUnit = 0);
	void bindDepthTexture(uint textureUnit = 0);
	void bindTexture(uint textureIndex, uint textureUnit);
	void deleteTextures();

private:
	static bool s_begun;
	bool m_begun;

	GLuint m_fbo;
	rde::vector<GLuint> m_textures;
	rde::vector<GLenum> m_drawBuffers;
	GLuint m_depthTexture;
};