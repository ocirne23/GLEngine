#pragma once

#include "Core.h"

#include "Graphics/GL/GLTypes.h"

struct VertexAttribute
{
	enum EFormat
	{
		EFormat_UNSIGNED_BYTE, EFormat_UNSIGNED_INT, EFormat_INT, EFormat_FLOAT
	};

	VertexAttribute(uint idx, const char* name, EFormat format, uint numElements, bool normalize = false) :
		m_attributeIndex(idx),
		m_attributeName(name),
		m_format(format),
		m_numElements(numElements),
		m_normalize(normalize)
	{}

	uint m_attributeIndex       = 0;
	const char* m_attributeName = NULL;
	EFormat m_format            = EFormat_UNSIGNED_BYTE;
	uint m_numElements          = 0;
	bool m_normalize            = false;
};


class GLVertexBuffer
{
public:
	friend class GLStateBuffer;

	GLVertexBuffer() {}
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

	bool m_initialized  = false;
	GLuint m_id         = 0;
	GLenum m_bufferType = 0;
	GLenum m_drawUsage  = 0;
};