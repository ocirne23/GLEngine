#pragma once

#include "Core.h"

#include "Graphics/GL/GLDefines.h"
#include "Graphics/GL/GLTypes.h"
#include "rde/vector.h"

class Pixmap;

class GLTextureArray
{
public:
	GLTextureArray() {}
	~GLTextureArray();
	GLTextureArray(const GLTextureArray& copyMe) = delete;

	void initialize(const rde::vector<Pixmap*>& pixmaps, uint numMipMaps = 4,
					GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
					GLint magFilter = GL_LINEAR,
					GLint textureWrapS = GL_CLAMP_TO_EDGE, GLint textureWrapT = GL_CLAMP_TO_EDGE);

	void bind(uint index = 0);
	void unbind(uint index = 0);

	bool isInitialized() { return m_initialized; }
	void setDimensions(uint width, uint height, uint numComponents, bool isFloatTexture);

	uint getWidth() const { return m_width; }
	uint getHeight() const { return m_height; }
	uint getNumComponents() const { return m_numComponents; }
	uint getTextureID() const { return m_textureID; }

private:

	bool m_initialized     = false;
	uint m_width           = 0;
	uint m_height          = 0;
	uint m_numMipmaps      = 0;
	uint m_numComponents   = 0;
	bool m_isFloatTexture  = false;
	bool m_generateMipMaps = false;
	GLuint m_textureID     = 0;
};