#pragma once

#include "Core.h"

#include "Graphics/GL/GLDefines.h"
#include "Graphics/GL/GLTypes.h"

class GLShader;

class GLTextureBuffer
{
public:
	friend class GLStateBuffer;

	GLTextureBuffer() {}
	~GLTextureBuffer();
	GLTextureBuffer(const GLTextureBuffer& copy) = delete;

	void initialize(const GLShader& shader, const char* samplerName, GLint textureIdx, GLenum sizedFormat, GLenum drawUsage = GL_STATIC_DRAW);
	void upload(uint numBytes, const void* data);
	void bind();

private:

	bool m_initialized           = false;
	const GLShader* m_shader     = NULL;
	GLenum m_drawUsage           = 0;
	GLint m_textureLoc           = 0;
	GLint m_textureIdx           = 0;
	GLuint m_textureID           = 0;
	GLuint m_bufferID            = 0;
	GLenum m_sizedInternalFormat = 0;
};