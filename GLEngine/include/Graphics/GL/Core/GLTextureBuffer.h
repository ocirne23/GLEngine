#pragma once

#ifndef ANDROID

#include "Core.h"
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

	void initialize(const GLShader& shader, const char* samplerName, GLint textureIdx, GLenum sizedFormat, GLenum drawUsage = GL_STATIC_DRAW);
	void upload(uint numBytes, const void* data);
	void bind();
private:

	bool m_initialized;

	GLenum m_drawUsage;
	GLint m_textureLoc;
	GLint m_textureIdx;
	GLuint m_textureID;

	GLuint m_bufferID;
	GLenum m_sizedInternalFormat;
};

#endif