#pragma once

#include "Core.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

class DBTexture;

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
	GLTextureArray(const GLTextureArray& copy) = delete;

	void startInit(uint width, uint height, uint depth, uint numComponents, bool isFloatTexture, uint numMipMaps = 4,
			ETextureMinFilter minFilter = ETextureMinFilter::LINEAR_MIPMAP_LINEAR,
			ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
			ETextureWrap textureWrapS = ETextureWrap::CLAMP_TO_EDGE,
			ETextureWrap textureWrapT = ETextureWrap::CLAMP_TO_EDGE);

	// Add a texture, returning the index it is placed in
	uint addTexture(const DBTexture& tex);
	void finishInit();

	void bind(uint index = 0);
	void unbind(uint index = 0);
	
	bool isInitialized() const    { return m_initialized; }
	uint getNumMipMaps() const    { return m_numMipmaps; }
	uint getTextureID() const     { return m_textureID; }

	uint getWidth() const         { return m_width; }
	uint getHeight() const        { return m_height; }
	uint getDepth() const         { return m_depth; }
	uint getNumComponents() const { return m_numComponents; }
	bool isFloatTexture() const   { return m_isFloatTexture; }

private:

	bool m_initialized      = false;
	uint m_numMipmaps       = 0;
	uint m_textureID        = 0;
	uint m_numTexturesAdded = 0;

	uint m_width          = 0;
	uint m_height         = 0;
	uint m_depth          = 0;
	uint m_numComponents  = 0;
	bool m_isFloatTexture = false;
};