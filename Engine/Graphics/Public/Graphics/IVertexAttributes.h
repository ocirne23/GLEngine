#pragma once

#include "Core.h"

class IBuffer;

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
	virtual void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize = false) = 0;
	virtual void bind(IBuffer& vertexBuffer) = 0;

protected:

	friend class IContext;
	virtual ~IVertexAttributes() {}
};