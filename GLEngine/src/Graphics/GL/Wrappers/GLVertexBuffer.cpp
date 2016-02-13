#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"

#include <assert.h>

GLVertexBuffer::~GLVertexBuffer()
{
	if (m_initialized)
		glDeleteBuffers(1, &m_id);
}

GLVertexBuffer::GLVertexBuffer(const GLVertexBuffer& copy)
{
	// Cannot copy after being initialized because there should be just one object keeping track of each gpu state
	assert(!m_initialized); 
}

void GLVertexBuffer::initialize(const Config& a_config)
{
	initialize(a_config.bufferType, a_config.drawUsage);
	if (a_config.vertexAttributes.size())
		setVertexAttributes(as_span(&a_config.vertexAttributes[0], a_config.vertexAttributes.size()));
}

void GLVertexBuffer::initialize(EBufferType a_bufferType, EDrawUsage a_drawUsage)
{
	if (m_initialized)
		glDeleteBuffers(1, &m_id);

	glGenBuffers(1, &m_id);
	m_bufferType = a_bufferType;
	m_drawUsage = a_drawUsage;
	m_initialized = true;
}

void GLVertexBuffer::upload(span<const byte> a_data)
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	if (!a_data.empty())
	{
		glBindBuffer(GLenum(m_bufferType), m_id);
		glBufferData(GLenum(m_bufferType), a_data.length_bytes(), a_data.data(), GLenum(m_drawUsage));
	}
}

void GLVertexBuffer::bind()
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	glBindBuffer(GLenum(m_bufferType), m_id);
}

void GLVertexBuffer::setVertexAttributes(span<const VertexAttribute> a_attributes)
{
	assert(GLStateBuffer::isBegun());
	assert(m_initialized);
	assert(!a_attributes.empty());

	uint64 offset = 0;
	uint stride = 0;

	if (a_attributes.size() == 1)
		stride = 0;
	else
	{
		for (uint i = 0; i < a_attributes.size(); ++i)
		{
			const VertexAttribute& attribute = a_attributes[i];
			stride += attribute.numElements * (attribute.format == VertexAttribute::EFormat::UNSIGNED_BYTE ? 1 : 4);
		}
	}

	for (uint i = 0; i < a_attributes.size(); ++i)
	{
		const VertexAttribute& attribute = a_attributes[i];

		bool isFloatType = (attribute.format == VertexAttribute::EFormat::FLOAT) || attribute.normalize;
		uint dataSize = (attribute.format == VertexAttribute::EFormat::UNSIGNED_BYTE) ? 1 : 4;
		dataSize *= attribute.numElements;
		glBindBuffer(GLenum(m_bufferType), m_id);
		if (isFloatType)
			glVertexAttribPointer(attribute.attributeIndex, attribute.numElements, GLenum(attribute.format), attribute.normalize, stride, rcast<GLvoid*>(offset));
		else
			glVertexAttribIPointer(attribute.attributeIndex, attribute.numElements, GLenum(attribute.format), stride, rcast<GLvoid*>(offset));
		glEnableVertexAttribArray(attribute.attributeIndex);
		offset += dataSize;
	}
}