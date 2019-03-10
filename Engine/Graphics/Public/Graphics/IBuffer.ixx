#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

export module Graphics.IBuffer;

import Core.ArrayView;

/*
enum class EBufferType
{
	VERTEX,
	INDEX,
	CONSTANT,
	TEXBUF
};*/

export enum class EBufferUsageType
{
	STATIC,
	DYNAMIC,
	STREAM
};

export class GRAPHICS_API IBuffer
{
public:

	virtual void             upload(array_view<const byte> data) = 0;
	virtual array_view<byte> map() = 0;
	virtual void             unmap() = 0;
	virtual uint64           getSizeBytes() const = 0;

protected:

	virtual ~IBuffer() {}
};