#pragma once

#include "Core.h"

#include "Graphics/GL/GLTypes.h"

struct VertexAttribute
{
	enum EFormat
	{
		EFormat_UNSIGNED_BYTE, EFormat_UNSIGNED_INT, EFormat_INT, EFormat_FLOAT
	};

	uint m_attributeIndex;
	const char* m_attributeName;
	EFormat m_format;
	uint m_numElements;
	bool m_normalize;

	VertexAttribute(uint idx, const char* name, EFormat format, uint numElements, bool normalize = false) :
		m_attributeIndex(idx),
		m_attributeName(name),
		m_format(format),
		m_numElements(numElements),
		m_normalize(normalize)
	{}
};


class GLVertexBuffer
{
public:
	friend class GLStateBuffer;

	GLVertexBuffer() : m_id(0), m_bufferType(0), m_drawUsage(0), m_initialized(false) {}
	~GLVertexBuffer();
	GLVertexBuffer(const GLVertexBuffer& copy) = delete;

	void initialize(GLenum bufferType, GLenum drawUsage);
	void upload(uint numBytes, const void* data = 0);
	void bind();
	void setVertexAttributes(uint numAttributes, VertexAttribute* attributes);
	void setAttribPointer(GLuint attributeIdx, GLenum type, uint valuesPerVertex,
		GLboolean normalized = false, GLboolean isIntegerType = false, GLuint stride = 0,
		GLuint offset = 0);

private:

	bool m_initialized;
	GLuint m_id;
	GLenum m_bufferType;
	GLenum m_drawUsage;
};