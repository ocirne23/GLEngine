#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

enum class ETextureType
{
	TEX1D,
	TEX2D,
	TEX3D,
	TEX1DARRAY,
	TEX2DARRAY,
	TEXCUBE,
	TEXCUBEARRAY,
	TEXBUFFER,
	TEX2DMS,
	TEX2DMSARRAY,
};

enum class EMultisampleType
{
	NONE,
	MSAA_2X,
	MSAA_4X,
	MSAA_8X,
	MSAA_2XFIXEDSAMPLE,
	MSAA_4XFIXEDSAMPLE,
	MSAA_8XFIXEDSAMPLE,
};

enum class ETextureMinFilter
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,
};

enum class ETextureCompareMode
{
	NONE,
	COMPARE_R_TO_TEXTURE,
};

enum class ETextureMagFilter
{
	NEAREST,
	LINEAR,
};

enum class ETextureWrap
{
	CLAMP_TO_EDGE,
	MIRRORED_REPEAT,
	REPEAT,
};

enum class ETextureComponentFormat
{
	F8,
	F16,
	F32,
	F8SNORM,
	F16SNORM,
	I8,
	I16,
	I32,
	UI8,
	UI16,
	UI32,
	BPTC_RGBA,
	BPTC_SRGBA,
	BPTC_RGB_FLOAT,
	BPTC_RGB_UNSIGNED_FLOAT,
	DXT1_RGB,
	DXT1A_RGBA,
	DXT3_RGBA,
	DXT5_RGBA,
	DXT1_SRGB,
	DXT1A_SRGBA,
	DXT3_SRGBA,
	DXT5_SRGBA,
};

enum class ETexturePixelDataFormat
{
	UNSIGNED_BYTE,
	BYTE,
	UNSIGNED_SHORT,
	SHORT,
	UNSIGNED_INT,
	INT,
	FLOAT,
	UNSIGNED_BYTE_3_3_2,
	UNSIGNED_SHORT_5_6_5,
	UNSIGNED_SHORT_4_4_4_4,
	UNSIGNED_SHORT_5_5_5_1,
	UNSIGNED_INT_8_8_8_8,
	UNSIGNED_INT_10_10_10_2,
};

struct TextureSizeSettings
{
	uint width = 0;
	uint height = 0;
	uint depth = 0;
	uint mipLevels = 0;
	ETextureComponentFormat format = ETextureComponentFormat::F8;
	uint numComponents = 0;
	EMultisampleType multisample = EMultisampleType::NONE;
};

struct TextureFilterSettings
{
	ETextureMinFilter minFilter = ETextureMinFilter::LINEAR;
	ETextureMagFilter magFilter = ETextureMagFilter::LINEAR;
	ETextureWrap sWrap = ETextureWrap::CLAMP_TO_EDGE;
	ETextureWrap tWrap = ETextureWrap::CLAMP_TO_EDGE;
	ETextureCompareMode compare = ETextureCompareMode::NONE;
};

struct TextureSettings
{
	TextureSizeSettings size;
	TextureFilterSettings filter;
};

class GRAPHICS_API ITexture
{
public:

	virtual void setOptions(const TextureSettings& options) = 0;
	virtual void loadTexture(void* data, ETexturePixelDataFormat format) = 0;

protected:

	virtual ~ITexture() {}
};