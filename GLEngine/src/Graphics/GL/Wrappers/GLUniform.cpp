#include "Graphics/GL/Wrappers/GLUniform.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"

#include <glm/glm.hpp>
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

template<>
void GLUniform<glm::vec2>::set(glm::vec2 value)
{
	assert(m_shader->isBegun());
	m_value = value;
	glUniform2fv(m_uniformLoc, 1, &value[0]);
}

template<>
void GLUniform<glm::vec3>::set(glm::vec3 value)
{
	assert(m_shader->isBegun());
	m_value = value;
	glUniform3fv(m_uniformLoc, 1, &value[0]);
}

template<>
void GLUniform<glm::mat3>::set(glm::mat3 value)
{
	assert(m_shader->isBegun());
	m_value = value;
	glUniformMatrix3fv(m_uniformLoc, 1, GL_FALSE, &value[0][0]);
}

template<>
void GLUniform<glm::mat4>::set(glm::mat4 value)
{
	assert(m_shader->isBegun());
	m_value = value;
	glUniformMatrix4fv(m_uniformLoc, 1, GL_FALSE, &value[0][0]);
}

template class GLUniform<float>;
template class GLUniform<glm::vec2>;
template class GLUniform<glm::vec3>;
template class GLUniform<glm::mat3>;
template class GLUniform<glm::mat4>;