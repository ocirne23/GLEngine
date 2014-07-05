#pragma once

#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\GLDefines.h"

class GLShader;

class GLTextureBuffer
{
public:
	friend class GLStateBuffer;

	GLTextureBuffer() : m_drawUsage(0), m_textureLoc(0), m_textureID(0) {}
	GLTextureBuffer(const GLTextureBuffer& copy) = delete;
	~GLTextureBuffer();

	void initialize(const GLShader& shader, const char* samplerName, GLint textureIdx, GLenum format, GLenum drawUsage = GL_STATIC_DRAW);
	void upload(unsigned int numBytes, const void* data);
	void bind();
private:

	bool m_initialized;

	GLenum m_drawUsage;
	GLint m_textureLoc;
	GLint m_textureIdx;
	GLuint m_textureID;

#ifndef ANDROID
	GLuint m_bufferID;
#else
	GLenum m_textureUnit;
	GLenum m_format;
	GLenum m_internalFormat;
#endif

};