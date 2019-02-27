#pragma once

#include "GraphicsAPI.h"

class ITexture;
enum class ETextureAttachment
{
	DEPTH,
	STENCIL,
	COLOR0,
	COLOR1,
	COLOR2,
	COLOR3,
	COLOR4,
	COLOR5
};

class GRAPHICS_API IFramebuffer
{
public:

	virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment) = 0;

protected:

	virtual ~IFramebuffer() {}

};