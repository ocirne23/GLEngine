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
	DXT1,
	DXT1A,
	DXT3,
	DXT5,
};

struct ITextureSettings
{
	uint width;
	uint height;
	uint depth;
	uint mipLevels;

	ETextureComponentFormat format;
	uint numComponents;
	uint textureBuffer;
	
	ETextureMinFilter minFilter;
	ETextureMagFilter magFilter;
	ETextureWrap sWrap;
	ETextureWrap tWrap;
	ETextureCompareMode compare;
	EMultisampleType multisample;
};

class GRAPHICS_API ITexture
{
public:

	virtual void setOptions(const ITextureSettings& options) = 0;

protected:

	virtual ~ITexture() {}
};