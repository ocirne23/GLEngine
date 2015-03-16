#include "Graphics/GL/Wrappers/GLStateBuffer.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

#include <assert.h>

bool GLStateBuffer::s_isBegun = false;

GLStateBuffer::~GLStateBuffer()
{
	if (m_initialized)
		glDeleteVertexArrays(1, &m_vao);
}

void GLStateBuffer::initialize()
{
	assert(!s_isBegun);
	assert(!m_initialized);
	glGenVertexArrays(1, &m_vao);
	m_initialized = true;
}

void GLStateBuffer::begin()
{
	assert(m_initialized);
	assert(!m_isBegun);
	assert(!s_isBegun);
	m_isBegun = true;
	s_isBegun = true;
	glBindVertexArray(m_vao);
}

void GLStateBuffer::end()
{
	assert(m_initialized);
	assert(m_isBegun);
	assert(s_isBegun);
	m_isBegun = false;
	s_isBegun = false;
	glBindVertexArray(0);
}