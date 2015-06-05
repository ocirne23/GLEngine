#pragma once

#include "Core.h"

#include "3rdparty/rde/vector.h"

#include "3rdparty/rde/rde_string.h"

class Pixmap;

class GLTextureArray
{
public:

	enum class ETextureMinFilter
	{
		NEAREST = 0x2600, // GL_NEAREST
		LINEAR  = 0x2601, // GL_LINEAR
		NEAREST_MIPMAP_NEAREST = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
		LINEAR_MIPMAP_NEAREST  = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
		NEAREST_MIPMAP_LINEAR  = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
		LINEAR_MIPMAP_LINEAR   = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
	};

	enum class ETextureMagFilter
	{
		NEAREST = 0x2600, // GL_NEAREST
		LINEAR  = 0x2601, // GL_LINEAR
	};

	enum class ETextureWrap
	{
		CLAMP_TO_EDGE   = 0x812F, // GL_CLAMP_TO_EDGE
		MIRRORED_REPEAT = 0x8370, // GL_MIRRORED_REPEAT
		REPEAT          = 0x2901, // GL_REPEAT
	};

public:

	GLTextureArray() {}
	~GLTextureArray();
	GLTextureArray(const GLTextureArray& copyMe) = delete;

	void initialize(const rde::vector<Pixmap>& pixmaps, uint numMipMaps = 4,
				ETextureMinFilter minFilter = ETextureMinFilter::LINEAR_MIPMAP_LINEAR,
				ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
				ETextureWrap textureWrapS = ETextureWrap::CLAMP_TO_EDGE,
				ETextureWrap textureWrapT = ETextureWrap::CLAMP_TO_EDGE);

	void initialize(const rde::vector<rde::string>& filePaths, uint numMipMaps = 4,
	                ETextureMinFilter minFilter = ETextureMinFilter::LINEAR_MIPMAP_LINEAR,
	                ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
	                ETextureWrap textureWrapS   = ETextureWrap::CLAMP_TO_EDGE, 
	                ETextureWrap textureWrapT   = ETextureWrap::CLAMP_TO_EDGE);

	void bind(uint index = 0);
	void unbind(uint index = 0);
	void setDimensions(uint width, uint height, uint numComponents, bool isFloatTexture);
	
	bool isInitialized() const    { return m_initialized; }
	uint getWidth() const         { return m_width; }
	uint getHeight() const        { return m_height; }
	uint getNumComponents() const { return m_numComponents; }
	uint getTextureID() const     { return m_textureID; }

private:

	bool m_initialized     = false;
	uint m_width           = 0;
	uint m_height          = 0;
	uint m_numMipmaps      = 0;
	uint m_numComponents   = 0;
	bool m_isFloatTexture  = false;
	bool m_generateMipMaps = false;
	uint m_textureID       = 0;
};