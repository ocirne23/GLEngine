#include "Graphics/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

void PerspectiveCamera::initialize(const glm::vec3& a_position, const glm::vec3& a_direction, float a_viewportWidth, float a_viewportHeight, float a_horizontalFov, float a_near, float a_far)
{
	Camera::initialize(a_position, a_direction, a_viewportWidth, a_viewportHeight, a_near, a_far);
	setHorizontalFieldOfView(a_horizontalFov);
}

void PerspectiveCamera::updateProjection()
{
	m_projectionMatrix = glm::perspective(m_vFieldOfView, m_viewportWidth / m_viewportHeight, m_near, m_far);
}

void PerspectiveCamera::setHorizontalFieldOfView(float a_fovh)
{
	m_hFieldOfView = a_fovh;
	m_vFieldOfView = glm::degrees(glm::atan(glm::tan(glm::radians(m_hFieldOfView) * (m_viewportHeight / m_viewportWidth))));
	updateProjection();
}

void PerspectiveCamera::setNearFar(float a_near, float a_far)
{
	m_near = a_near;
	m_far = a_far;
	updateProjection();
}

void PerspectiveCamera::resize(int a_width, int a_height)
{
	m_viewportWidth = float(a_width);
	m_viewportHeight = float(a_height);
	updateProjection();
}