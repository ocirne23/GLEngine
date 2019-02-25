#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

/*
enum class EBufferType
{
	VERTEX,
	INDEX,
	CONSTANT,
	TEXBUF
};*/

enum class EBufferUsageType
{
	STATIC,
	DYNAMIC,
	STREAM
};

class GRAPHICS_API IBuffer
{
public:

	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) = 0;
	virtual void upload(span<const byte> data) = 0;
	virtual span<byte> map() = 0;
	virtual void unmap() = 0;
	virtual uint64 getSizeBytes() const = 0;

private:

	friend class IContext;
	IBuffer();
};