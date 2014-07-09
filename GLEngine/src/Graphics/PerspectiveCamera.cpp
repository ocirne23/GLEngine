#include "Graphics\PerspectiveCamera.h"

#include <glm\gtc\matrix_transform.hpp>

void PerspectiveCamera::initialize(const glm::vec3& position, const glm::vec3& direction, float viewportWidth, float viewportHeight, float horizontalFov, float near, float far)
{
	Camera::initialize(position, direction, viewportWidth, viewportHeight, near, far);
	setHorizontalFieldOfView(horizontalFov);
}

PerspectiveCamera::~PerspectiveCamera()
{
}

void PerspectiveCamera::updateProjection()
{
	m_projectionMatrix = glm::perspective(m_vFieldOfView, m_viewportWidth / m_viewportHeight, m_near, m_far);
}

void PerspectiveCamera::setHorizontalFieldOfView(float fovh)
{
	m_hFieldOfView = fovh;
	m_vFieldOfView = glm::degrees(glm::atan(glm::tan(glm::radians(m_hFieldOfView) * (m_viewportHeight / m_viewportWidth))));
	updateProjection();
}

void PerspectiveCamera::setNearFar(float near, float far)
{
	m_near = near;
	m_far = far;
	updateProjection();
}

void PerspectiveCamera::resize(int width, int height)
{
	m_viewportWidth = float(width);
	m_viewportHeight = float(height);
	updateProjection();
}