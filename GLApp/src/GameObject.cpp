#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

void GameObject::render(GLRenderer& a_renderer, bool a_depthOnly)
{
	if (m_dirty) 
		updateTransform();
	m_scene->render(a_renderer, m_transform, a_depthOnly);
}

void GameObject::setPosition(const glm::vec3 & a_position) 
{ 
	m_position = a_position; 
	m_dirty = true; 
}

void GameObject::setScale(float a_scale) 
{ 
	m_scale = a_scale; 
	m_dirty = true; 
}

void GameObject::setRotation(const glm::vec3 & a_axis, float a_degrees) 
{ 
	m_axisRotation = glm::vec4(a_axis, a_degrees); 
	m_dirty = true; 
}

void GameObject::updateTransform()
{
	const glm::mat4 translationMat = glm::translate(glm::mat4(1), m_position);
	const glm::mat4 rotatedMat = glm::rotate(translationMat, m_axisRotation.w, glm::vec3(m_axisRotation));
	m_transform = glm::scale(rotatedMat, glm::vec3(m_scale));
	m_dirty = false;
}

