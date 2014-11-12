#pragma once

#include "Core.h"

#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\GLDefines.h"

class Pixmap;
class FileHandle;

class GLTexture
{
public:
	GLTexture() : m_textureID(0), m_width(0), m_height(0), m_numComponents(0), m_initialized(false) {};
	~GLTexture();
	GLTexture(const GLTexture& copy) = delete;

	void initialize(const FileHandle& filePath,
		GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR,
		GLint GLTextureWrapS = GL_REPEAT, GLint GLTextureWrapT = GL_REPEAT);

	void bind(uint index = 0);
	void unbind(uint index = 0);

	bool isLoaded() const					{ return m_textureID != 0; };
	GLuint getTextureID() const				{ return m_textureID; }
	uint getWidth() const			{ return m_width; }
	uint getHeight() const			{ return m_height; }
	byte getNumComponents() const	{ return m_numComponents; }

private:
	void setupGLTexture(const Pixmap& pixmap, GLint minFilter, GLint magFilter,
		GLint GLTextureWrapS, GLint GLTextureWrapT);

private:

	bool m_initialized;

	GLuint m_textureID;
	uint m_width;
	uint m_height;
	byte m_numComponents;
};