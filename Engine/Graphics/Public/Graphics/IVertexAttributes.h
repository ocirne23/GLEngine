#pragma once

#include "Core.h"
#include "GraphicsAPI.h"
#include "Core/PimplPtr.h"

class IBuffer;

enum class EVertexAttributeFormat
{
	FLOAT,
	UNSIGNED_BYTE,
	UNSIGNED_INT,
	INT
};

class GLVertexAttributes;

class GRAPHICS_API IVertexAttributes
{
public:

	IVertexAttributes();
	explicit operator GLVertexAttributes&() { return *m_impl; }

	void reset();
	void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize = false);
	void bind(IBuffer& vertexBuffer);

private:

	PimplPtr<GLVertexAttributes> m_impl;
};