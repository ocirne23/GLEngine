#pragma once

#include "Core.h"

#include "rde\vector.h"

#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\GLDefines.h"

class Pixmap;

class GLTextureArray
{
public:
	GLTextureArray() : m_initialized(false), m_width(0), m_height(0),
		m_numMipmaps(0), m_numComponents(0), m_generateMipMaps(0), m_textureID(0) {}
	~GLTextureArray();
	GLTextureArray(const GLTextureArray& copyMe) = delete;

	void initialize(const rde::vector<Pixmap*>& pixmaps, uint numMipMaps = 5,
		GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR,
		GLint textureWrapS = GL_REPEAT, GLint textureWrapT = GL_REPEAT);

	void bind(uint index = 0);
	void unbind(uint index = 0);

	void setDimensions(uint width, uint height, uint numComponents, bool isFloatTexture);

	inline uint getWidth() const		{ return m_width; }
	inline uint getHeight() const		{ return m_height; }
	inline uint getNumComponents() const{ return m_numComponents; }
	inline uint getTextureID() const	{ return m_textureID; }

private:

	bool m_initialized;

	uint m_width;
	uint m_height;
	uint m_numMipmaps;
	uint m_numComponents;
	bool m_isFloatTexture;

	bool m_generateMipMaps;

	GLuint m_textureID;
};