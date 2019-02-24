#pragma once

#include "Core.h"
#include "GraphicsAPI.h"
#include "Core/Pimpl.h"

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

class GLBuffer;

class GRAPHICS_API IBuffer
{
public:

	IBuffer();
	explicit operator GLBuffer&() { return *m_impl; }

	void initialize(uint64 sizeBytes, EBufferUsageType usageType);
	void upload(span<const byte> data);
	span<byte> map();
	void unmap();
	uint64 getSizeBytes() const;

private:

	Pimpl<GLBuffer> m_impl;
};