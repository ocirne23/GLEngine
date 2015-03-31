#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"

#include <assert.h>

GLVertexBuffer::~GLVertexBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_id);
}

void GLVertexBuffer::initialize(EBufferType a_bufferType, EDrawUsage a_drawUsage)
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
		glBindBuffer((GLenum) m_bufferType, m_id);
		glBufferData((GLenum) m_bufferType, a_numBytes, a_data, (GLenum) m_drawUsage);
	}
}

void GLVertexBuffer::bind()
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	glBindBuffer((GLenum) m_bufferType, m_id);
}

void GLVertexBuffer::setVertexAttributes(uint a_numAttributes, VertexAttribute* a_attributes)
{
	assert(GLStateBuffer::isBegun());
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
			stride += attribute->numElements * (attribute->format == VertexAttribute::EFormat::UNSIGNED_BYTE ? 1 : 4);
		}
	}

	for (uint i = 0; i < a_numAttributes; ++i)
	{
		const VertexAttribute& attribute = *(a_attributes + i);

		bool isFloatType = (attribute.format == VertexAttribute::EFormat::FLOAT) || attribute.normalize;
		uint dataSize = (attribute.format == VertexAttribute::EFormat::UNSIGNED_BYTE) ? 1 : 4;
		dataSize *= attribute.numElements;

		glBindBuffer((GLenum) m_bufferType, m_id);
		if (isFloatType)
			glVertexAttribPointer(attribute.attributeIndex, attribute.numElements, (GLenum) attribute.format, attribute.normalize, stride, (const GLvoid*) offset);
		else
			glVertexAttribIPointer(attribute.attributeIndex, attribute.numElements, (GLenum) attribute.format, stride, (const GLvoid*) offset);
		glEnableVertexAttribArray(attribute.attributeIndex);
		offset += dataSize;
	}
}