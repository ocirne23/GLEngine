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

class GLBuffer;

class GRAPHICS_API IBuffer
{
public:

	IBuffer();
	IBuffer(const IBuffer&) = delete;
	~IBuffer();
	explicit operator GLBuffer&() { return *m_impl; }

	void initialize(uint64 sizeBytes, EBufferUsageType usageType);
	void upload(span<const byte> data);
	span<byte> map();
	void unmap();
	uint64 getSizeBytes() const;

private:

	owner<GLBuffer*> m_impl = nullptr;
};