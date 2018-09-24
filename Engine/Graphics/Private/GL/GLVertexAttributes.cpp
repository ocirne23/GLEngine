#include "GLVertexAttributes.h"

#include "GL.h"
#include "GLUtils.h"
#include "GLVertexBuffer.h"

void GLVertexAttributes::initialize()
{
	if (m_initialized)
	{
		glDeleteVertexArrays(1, &m_vaoID);
		reset();
	}
	glCreateVertexArrays(1, &m_vaoID);
	m_initialized = true;
}

void GLVertexAttributes::addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize)
{
	m_attributes.emplace_back(format, numElements, normalize);
}

void GLVertexAttributes::reset()
{
	m_attributes.clear();
}

void GLVertexAttributes::bind(GLVertexBuffer& buffer)
{
	uint stride = 0;
	for (const auto& attrib : m_attributes)
		stride += GLUtils::getGLVertexAttributeSize(attrib.format) * attrib.numElements;

	uint offset = 0;
	for (uint i = 0; i < m_attributes.size(); ++i)
	{
		const auto& attrib = m_attributes[i];
		uint elemSize = GLUtils::getGLVertexAttributeSize(attrib.format);
		glEnableVertexArrayAttrib(m_vaoID, i);
		glVertexArrayAttribFormat(m_vaoID, i, attrib.numElements, GLUtils::getGLVertexAttributeType(attrib.format), attrib.normalize, offset);
		glVertexArrayVertexBuffer(m_vaoID, i, buffer.getID(), offset, stride);
		offset += attrib.numElements * elemSize;
	}
}