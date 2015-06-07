#pragma once

#include "Core.h"

class Pixmap;

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

public:

	GLTexture() {}
	~GLTexture();
	GLTexture(const GLTexture& copy) = delete;

	void initialize(const Pixmap& pixmap,
				ETextureMinFilter minFilter = ETextureMinFilter::LINEAR_MIPMAP_LINEAR,
				ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
				ETextureWrap textureWrapS = ETextureWrap::REPEAT,
				ETextureWrap textureWrapT = ETextureWrap::REPEAT);

	void initialize(const char* filePath,
					ETextureMinFilter minFilter = ETextureMinFilter::LINEAR_MIPMAP_LINEAR,
					ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
					ETextureWrap textureWrapS = ETextureWrap::REPEAT,
					ETextureWrap textureWrapT = ETextureWrap::REPEAT);

	void bind(uint index = 0);
	void unbind(uint index = 0);

	bool isLoaded() const         { return m_textureID != 0; }
	uint getTextureID() const     { return m_textureID; }
	uint getWidth() const         { return m_width; }
	uint getHeight() const        { return m_height; }
	byte getNumComponents() const { return m_numComponents; }

private:

	bool m_initialized   = false;
	uint m_textureID     = 0;
	uint m_width         = 0;
	uint m_height        = 0;
	byte m_numComponents = 0;
};