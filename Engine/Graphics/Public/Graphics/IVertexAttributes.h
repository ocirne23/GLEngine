#pragma once

#include "Core.h"

enum class EVertexAttributeFormat
{
	FLOAT,
	UNSIGNED_BYTE,
	UNSIGNED_INT,
	INT
};

class IVertexAttributes
{
public:

	virtual void reset() = 0;
	virtual void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize) = 0;

protected:

	friend class IContext;
	virtual ~IVertexAttributes() {}
};