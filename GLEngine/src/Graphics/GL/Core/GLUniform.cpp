#include "Graphics/GL/Core/GLUniform.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Core/GLShader.h"

#include <assert.h>

template <typename T>
void GLUniform<T>::initialize(const GLShader& a_shader, const char* a_uniformName)
{
	assert(a_shader.isBegun());
	m_shader = &a_shader;
	m_uniformLoc = glGetUniformLocation(a_shader.getID(), a_uniformName);
}

template<>
void GLUniform<float>::set(float value)
{
	assert(m_shader->isBegun());
	m_value = value;
	glUniform1f(m_uniformLoc, value);
}

template class GLUniform<float>;