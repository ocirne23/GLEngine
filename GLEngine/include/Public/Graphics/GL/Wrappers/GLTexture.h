#pragma once

#include "Core.h"

class DBTexture;

class GLTexture
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

	enum class EMultiSampleType
	{
		NONE = 0,
		MSAA_2X = 2,
		MSAA_4X = 4,
		MSAA_8X = 8,
		MSAA_16X = 16
	};

public:

	GLTexture() {}
	~GLTexture();
	GLTexture(const GLTexture& copy) = delete;

	void initialize(const DBTexture& texture, uint numMipmaps = 0,
				ETextureMinFilter minFilter = ETextureMinFilter::NEAREST,
				ETextureMagFilter magFilter = ETextureMagFilter::NEAREST,
				ETextureWrap textureWrapS = ETextureWrap::CLAMP_TO_EDGE,
				ETextureWrap textureWrapT = ETextureWrap::CLAMP_TO_EDGE);

	void bind(uint index = 0);
	void unbind(uint index = 0);

	bool isLoaded() const         { return m_textureID != 0; }
	uint getTextureID() const     { return m_textureID; }
	uint getWidth() const         { return m_width; }
	uint getHeight() const        { return m_height; }
	byte getNumComponents() const { return m_numComponents; }
	bool isInitialized() const    { return m_initialized; }

private:

	bool m_initialized   = false;
	uint m_textureID     = 0;
	uint m_width         = 0;
	uint m_height        = 0;
	byte m_numComponents = 0;
};