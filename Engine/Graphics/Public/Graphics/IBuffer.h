#pragma once

#include "Core.h"

enum class EBufferType
{
	VERTEX,
	INDEX,
	CONSTANT,
	TEXBUF
};

enum class EBufferUsageType
{
	STATIC,
	DYNAMIC,
	STREAM
};

class IBuffer
{
public:

	virtual EBufferType getType() const = 0;
	virtual void initialize(uint64 sizeBytes, EBufferUsageType usageType) = 0;
	virtual void upload(span<const byte> data) = 0;
	virtual void bind() = 0;

protected:

	friend class IContext;
	virtual ~IBuffer() {}
};