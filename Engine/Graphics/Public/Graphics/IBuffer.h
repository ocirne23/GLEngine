#pragma once

#include "Core.h"

enum class EBufferType
{
	VERTEX,
	INDEX,
	CONSTANT,
	TEXBUF
};

enum class EBufferDataAccessType
{
	STATIC,
	DYNAMIC,
	STREAM
};

class IBuffer
{
public:

	virtual EBufferType getType() const = 0;

protected:

	friend class IContext;
	virtual ~IBuffer() {}
};