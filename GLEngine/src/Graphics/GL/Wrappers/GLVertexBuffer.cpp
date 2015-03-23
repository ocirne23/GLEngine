#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"

#include <assert.h>

GLVertexBuffer::~GLVertexBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_id);
}

void GLVertexBuffer::initialize(GLenum a_bufferType, GLenum a_drawUsage)
{
	assert(!m_initialized);

	glGenBuffers(1, &m_id);
	m_bufferType = a_bufferType;
	m_drawUsage = a_drawUsage;
	m_initialized = true;
}

void GLVertexBuffer::upload(uint a_numBytes, const void* a_data)
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	if (a_numBytes)
	{
		glBindBuffer(m_bufferType, m_id);
		glBufferData(m_bufferType, a_numBytes, a_data, m_drawUsage);
	}
}

void GLVertexBuffer::bind()
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	glBindBuffer(m_bufferType, m_id);
}

void GLVertexBuffer::setVertexAttributes(uint a_numAttributes, VertexAttribute* a_attributes)
{
	assert(m_initialized);
	assert(a_numAttributes);

	uint offset = 0;
	uint stride = 0;

	if (a_numAttributes == 1)
		stride = 0;
	else
	{
		for (uint i = 0; i < a_numAttributes; ++i)
		{
			const VertexAttribute* attribute = a_attributes + i;
			stride += attribute->m_numElements * (attribute->m_format == VertexAttribute::EFormat::UNSIGNED_BYTE ? 1 : 4);
		}
	}

	for (uint i = 0; i < a_numAttributes; ++i)
	{
		const VertexAttribute* attribute = a_attributes + i;

		GLenum type;
		uint dataSize;
		bool isIntegerType = false;
		switch (attribute->m_format)
		{
		case VertexAttribute::EFormat::UNSIGNED_BYTE:
			type = GL_UNSIGNED_BYTE;
			dataSize = 1;
			isIntegerType = true;
			break;
		case VertexAttribute::EFormat::UNSIGNED_INT:
			type = GL_UNSIGNED_INT;
			isIntegerType = true;
			dataSize = 4;
			break;
		case VertexAttribute::EFormat::INT:
			type = GL_INT;
			isIntegerType = true;
			dataSize = 4;
			break;
		case VertexAttribute::EFormat::FLOAT:
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

void GLVertexBuffer::setAttribPointer(GLuint a_attributeIdx, GLenum a_type, uint a_valuesPerVertex,
	                                  GLboolean a_normalized, GLboolean a_isIntegerType, GLuint a_stride, GLuint a_offset)
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);

	glBindBuffer(m_bufferType, m_id);
	if (a_isIntegerType)
		glVertexAttribIPointer(a_attributeIdx, a_valuesPerVertex, a_type, a_stride, (const GLvoid*) a_offset);
	else
		glVertexAttribPointer(a_attributeIdx, a_valuesPerVertex, a_type, a_normalized, a_stride, (const GLvoid*) a_offset);

	glEnableVertexAttribArray(a_attributeIdx);
}
