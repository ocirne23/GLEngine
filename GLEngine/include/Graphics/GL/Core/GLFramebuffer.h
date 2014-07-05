#pragma once

#include "Graphics\GL\GLTypes.h"

#include "rde\vector.h"

class GLFramebuffer
{
public:
	GLFramebuffer();
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& copy) = delete;

	void addFramebufferTexture(GLenum format, GLenum attachment, unsigned int width, unsigned int height);
	void setDepthbufferTexture(GLenum format, unsigned int width, unsigned int height);

	void end();
	void begin();

	void bindTextures(unsigned int fromTextureUnit = 0);
	void bindDepthTexture(unsigned int textureUnit = 0);
	void bindTexture(unsigned int textureIndex, unsigned int textureUnit);
	void deleteTextures();

private:

	static bool s_begun;
	bool m_begun;

	GLuint m_fbo;
	rde::vector<GLuint> m_textures;
	rde::vector<GLenum> m_drawBuffers;
	GLuint m_depthTexture;
};