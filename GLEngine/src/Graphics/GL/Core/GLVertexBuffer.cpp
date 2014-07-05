#include "Graphics\GL\Core\GLVertexBuffer.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLStateBuffer.h"

#include <assert.h>

GLVertexBuffer::~GLVertexBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_id);
}

void GLVertexBuffer::initialize(GLenum bufferType, GLenum drawUsage)
{
	assert(!m_initialized);
	glGenBuffers(1, &m_id);
	m_bufferType = bufferType;
	m_drawUsage = drawUsage;
	m_initialized = true;
}

void GLVertexBuffer::upload(uint numBytes, const void* data)
{
	print("Vertex buffer upload: %f kb \n", numBytes / 1000.0f);
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	glBindBuffer(m_bufferType, m_id);
	glBufferData(m_bufferType, numBytes, data, m_drawUsage);
}

void GLVertexBuffer::bind()
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	glBindBuffer(m_bufferType, m_id);
}

void GLVertexBuffer::setVertexAttributes(uint numAttributes, VertexAttribute* attributes)
{
	assert(m_initialized);

	uint offset = 0;
	uint stride = 0;

	if (numAttributes == 1)
	{
		stride = 0;
	}
	else
	{
		for (uint i = 0; i < numAttributes; ++i)
		{
			const VertexAttribute* attribute = attributes + i;
			stride += attribute->m_numElements * (attribute->m_format == VertexAttribute::Format::UNSIGNED_BYTE ? 1 : 4);
		}
	}

	for (uint i = 0; i < numAttributes; ++i)
	{
		const VertexAttribute* attribute = attributes + i;

		GLenum type;
		uint dataSize;
		bool isIntegerType = false;
		switch (attribute->m_format)
		{
		case VertexAttribute::Format::UNSIGNED_BYTE:
			type = GL_UNSIGNED_BYTE;
			dataSize = 1;
			isIntegerType = true;
			break;
		case VertexAttribute::Format::UNSIGNED_INT:
			type = GL_UNSIGNED_INT;
			isIntegerType = true;
			dataSize = 4;
			break;
		case VertexAttribute::Format::INT:
			type = GL_INT;
			isIntegerType = true;
			dataSize = 4;
			break;
		case VertexAttribute::Format::FLOAT:
			type = GL_FLOAT;
			dataSize = 4;
			break;
		default:
			assert(false);
			type = GL_FLOAT;
			dataSize = 4;
			break;
		}
		if (attribute->m_normalize)
			isIntegerType = false;

		dataSize *= attribute->m_numElements;
		setAttribPointer(attribute->m_attributeIndex, type, attribute->m_numElements, attribute->m_normalize, isIntegerType, stride, offset);
		offset += dataSize;
	}
}

void GLVertexBuffer::setAttribPointer(GLuint attributeIdx, GLenum type, uint valuesPerVertex,
	GLboolean normalized, GLboolean isIntegerType, GLuint stride, GLuint offset)
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);

	glBindBuffer(m_bufferType, m_id);
	if (isIntegerType)
		glVertexAttribIPointer(attributeIdx, valuesPerVertex, type, stride, (const GLvoid*) offset);
	else
		glVertexAttribPointer(attributeIdx, valuesPerVertex, type, normalized, stride, (const GLvoid*) offset);

	glEnableVertexAttribArray(attributeIdx);
}
