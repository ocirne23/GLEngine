#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

class IBuffer;

enum class EVertexAttributeFormat
{
	FLOAT,
	UNSIGNED_BYTE,
	UNSIGNED_INT,
	INT
};

class GRAPHICS_API IVertexAttributes
{
public:

	IVertexAttributes();
	IVertexAttributes(const IVertexAttributes&) = delete;
	~IVertexAttributes();
	explicit operator GLVertexAttributes&() { return *m_impl; }

	void reset();
	void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize = false);
	void bind(IBuffer& vertexBuffer);

private:

	owner<GLVertexAttributes*> m_impl = nullptr;
};