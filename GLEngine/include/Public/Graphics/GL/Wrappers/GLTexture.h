#pragma once

#include "Core.h"

class DBTexture;

class GLTexture
{
public:

	// https://www.opengl.org/sdk/docs/man/html/glTexStorage2D.xhtml

	enum class ESizedFormat
	{
		R8       = 0x8229, // GL_R8,
		R16      = 0x822A, // GL_R16,
		R16F     = 0x822D, // GL_R16F,
		R32F     = 0x822E, // GL_R32F,
		R8I      = 0x8231, // GL_R8I,
		R16I     = 0x8233, // GL_R16I,
		R32I     = 0x8235, // GL_R32I,
		R8UI     = 0x8232, // GL_R8UI,
		R16UI    = 0x8234, // GL_R16UI,
		R32UI    = 0x8236, // GL_R32UI,

		RG8      = 0x822B, // GL_RG8,
		RG16     = 0x822C, // GL_RG16,
		RG16F    = 0x822F, // GL_RG16F,
		RG32F    = 0x8230, // GL_RG32F,
		RG8I     = 0x8237, // GL_RG8I,
		RG16I    = 0x8239, // GL_RG16I,
		RG32I    = 0x823B, // GL_RG32I,
		RG8UI    = 0x8238, // GL_RG8UI,
		RG16UI   = 0x823A, // GL_RG16UI,
		RG32UI   = 0x823C, // GL_RG32UI,

		RGB8     = 0x8051, // GL_RGB8,
		RGB16    = 0x8054, // GL_RGB16
		RGB16F   = 0x881B, // GL_RGB16F
		RGB32F   = 0x8815, // GL_RGB32F
		RGB8I    = 0x8D8F, // GL_RGB8I
		RGB16I   = 0x8D89, // GL_RGB16I
		RGB32I   = 0x8D83, // GL_RGB32I
		RGB8UI   = 0x8D7D, // GL_RGB8UI
		RGB16UI  = 0x8D77, // GL_RGB16UI
		RGB32UI  = 0x8D71, // GL_RGB32UI

		RGBA8    = 0x8058, // GL_RGBA8,
		RGBA16   = 0x805B, // GL_RGBA16,
		RGBA16F  = 0x881A, // GL_RGBA16F
		RGBA32F  = 0x8814, // GL_RGBA32F
		RGBA8I   = 0x8D8E, // GL_RGBA8I,
		RGBA16I  = 0x8D88, // GL_RGBA16I
		RGBA32I  = 0x8D82, // GL_RGBA32I
		RGBA8UI  = 0x8D7C, // GL_RGBA8UI
		RGBA16UI = 0x8D76, // GL_RGBA16UI
		RGBA32UI = 0x8D70, // GL_RGBA32UI

		DEPTH16  = 0x81A5, // GL_DEPTH_COMPONENT16
		DEPTH24  = 0x81A6, // GL_DEPTH_COMPONENT24
		DEPTH32  = 0x81A7  // GL_DEPTH_COMPONENT32
	};

	enum class EMultiSampleType
	{
		NONE = 0,
		MSAA_2X = 2,
		MSAA_4X = 4,
		MSAA_8X = 8,
		// MSAA_16X = 16
	};

	enum class ETextureMinFilter
	{
		NEAREST                = 0x2600, // GL_NEAREST
		LINEAR                 = 0x2601, // GL_LINEAR
		NEAREST_MIPMAP_NEAREST = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
		LINEAR_MIPMAP_NEAREST  = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
		NEAREST_MIPMAP_LINEAR  = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
		LINEAR_MIPMAP_LINEAR   = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
	};

	enum class ETextureCompareMode
	{
		NONE = 0,					  // GL_NONE
		COMPARE_R_TO_TEXTURE = 0x884E // GL_COMPARE_R_TO_TEXTURE
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

	void initialize(ESizedFormat format, uint width, uint height, 
		EMultiSampleType multiSampleType = EMultiSampleType::NONE,
		ETextureCompareMode compareMode = ETextureCompareMode::NONE,
		ETextureMinFilter minFilter = ETextureMinFilter::LINEAR,
		ETextureMagFilter magFilter = ETextureMagFilter::LINEAR,
		ETextureWrap textureWrapS = ETextureWrap::CLAMP_TO_EDGE,
		ETextureWrap textureWrapT = ETextureWrap::CLAMP_TO_EDGE);

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
	EMultiSampleType getMultiSampleType() const { return m_multiSampleType; }

private:

	bool m_initialized   = false;
	uint m_textureID     = 0;
	uint m_width         = 0;
	uint m_height        = 0;
	byte m_numComponents = 0;
	EMultiSampleType m_multiSampleType = EMultiSampleType::NONE;
};