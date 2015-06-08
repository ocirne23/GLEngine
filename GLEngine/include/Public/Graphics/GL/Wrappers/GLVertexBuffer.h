#pragma once

#include "Core.h"

struct VertexAttribute
{
	enum class EFormat
	{
		UNSIGNED_BYTE = 0x1401, // GL_UNSIGNED_BYTE
		UNSIGNED_INT  = 0x1405, // GL_UNSIGNED_INT
		INT           = 0x1404, // GL_INT
		FLOAT         = 0x1406  // GL_FLOAT
	};

	VertexAttribute(uint idx, const char* name, EFormat format, uint numElements, bool normalize = false) :
		attributeIndex(idx),
		attributeName(name),
		format(format),
		numElements(numElements),
		normalize(normalize)
	{}

	uint attributeIndex       = 0;
	const char* attributeName = NULL;
	EFormat format            = EFormat::UNSIGNED_BYTE;
	uint numElements          = 0;
	bool normalize            = false;
};

class GLVertexBuffer
{
public:
	friend class GLStateBuffer;

	enum class EBufferType
	{
		ARRAY         = 0x8892, // GL_ARRAY_BUFFER
		ELEMENT_ARRAY = 0x8893  //GL_ELEMENT_ARRAY_BUFFER
	};

	enum class EDrawUsage
	{
		STATIC  = 0x88E4, // GL_STATIC_DRAW,
		DYNAMIC = 0x88E8, // GL_DYNAMIC_DRAW,
		STREAM  = 0x88E0  // GL_STREAM_DRAW
	};

public:

	GLVertexBuffer() {}
	~GLVertexBuffer();
	GLVertexBuffer(const GLVertexBuffer& copy) = delete;

	void initialize(EBufferType bufferType, EDrawUsage drawUsage);
	void upload(uint numBytes, const void* data = 0);
	void bind();
	void setVertexAttributes(uint numAttributes, VertexAttribute* attributes);
	bool isInitialized() const { return m_initialized; }

private:

	bool m_initialized       = false;
	uint m_id                = 0;
	EBufferType m_bufferType = EBufferType::ARRAY;
	EDrawUsage m_drawUsage   = EDrawUsage::DYNAMIC;
};