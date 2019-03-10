#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

export module Graphics.IContext;

import Graphics.IBuffer;
import Graphics.ITexture;

export enum class EContextType
{
	OPENGL
};

export class GRAPHICS_API IContext
{
public:

	virtual EContextType     getContextType() const = 0;
	virtual owner<IBuffer*>  createBuffer(uint64 size, const EBufferUsageType& type) = 0;
	virtual void             destroyBuffer(owner<IBuffer*>& buffer) = 0;
	virtual owner<ITexture*> createTexture(const ETextureType& type) = 0;
	virtual void             destroyTexture(owner<ITexture*>& texture) = 0;

protected:

	virtual ~IContext() {}
};