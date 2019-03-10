#pragma once

#include "GraphicsAPI.h"

export module Graphics.IFramebuffer;

class ITexture;
export enum class ETextureAttachment
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

export enum class EFramebufferTarget
{
	DRAW,
	READ,
	DRAWREAD,
};

export class __declspec(dllexport) IFramebuffer
{
public:

	virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment) = 0;
	//virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment) = 0;
	virtual void bind(EFramebufferTarget target) = 0;

protected:

	virtual ~IFramebuffer() {}

};