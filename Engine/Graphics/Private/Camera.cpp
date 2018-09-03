#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Camera::initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp)
{
	m_projection = projection;
	m_near = near;
	m_far = far;
	m_width = width;
	m_height = height;
	m_position = glm::vec3();
	m_direction = glm::vec3(0, 0, -1);
	m_up = glm::vec3(0, 1, 0);
	m_worldUp = worldUp;
	setHorizontalFov(horizontalFov);
	updateMatrices();
}

void Camera::updateMatrices()
{
	const glm::vec3 side = glm::cross(m_direction, m_worldUp);
	m_up = glm::cross(side, m_direction);
	if (m_projection == EProjection::PERSPECTIVE)
		m_projectionMatrix = glm::perspective(glm::radians(m_vFieldOfView), m_width / m_height, m_near, m_far);
	else if (m_projection == EProjection::ORTHOGRAPHIC)
		m_projectionMatrix = glm::ortho(m_width / 2, -m_width / 2, -m_height / 2, m_height / 2, m_near, m_far);
	m_viewMatrix = glm::lookAt(m_position, m_position + m_direction, m_up);
	m_combinedMatrix = m_projectionMatrix * m_viewMatrix;
	m_frustum.calculateFrustum(m_combinedMatrix);
}

void Camera::setPosition(const glm::vec3& position)
{
	m_position = position;
}

void Camera::setDirection(const glm::vec3& direction)
{
	m_direction = direction;
}

void Camera::setHorizontalFov(float hFov)
{
	m_hFieldOfView = hFov;
	if (m_width == m_height)
		m_vFieldOfView = hFov;
	else
		m_vFieldOfView = glm::degrees(glm::atan(glm::tan(glm::radians(m_hFieldOfView) * (m_height / m_width))));
}

void Camera::setWorldUp(const glm::vec3& worldUp)
{
	m_worldUp = worldUp;
}

void Camera::lookAtPoint(const glm::vec3& position)
{
	m_direction = glm::normalize(position - m_position);
}

void Camera::translateAbsolute(const glm::vec3& translation)
{
	m_position += translation;
}

void Camera::translateRelative(const glm::vec3& translation)
{
	const float angle = getRotationRadXZ();
	const float xTrans = translation.x * glm::cos(angle) + translation.z * glm::sin(angle);
	const float zTrans = translation.z * glm::cos(angle) - translation.x * glm::sin(angle);
	translateAbsolute(glm::vec3(-xTrans, translation.y, -zTrans));
}

void Camera::rotateAbsolute(float angle, const glm::vec3& axis)
{
	const glm::vec3 tmp = m_direction;
	m_direction = glm::rotate(m_direction, angle, axis);
	//limit vertical look movement
	if (m_direction.y > 0.99f || m_direction.y < -0.99f)
	{
		m_direction = tmp;
		return;
	}
	//recalculate up vector.
	const glm::vec3 side = glm::cross(m_direction, m_worldUp);
	m_up = glm::cross(side, m_direction);
}

void Camera::rotateRelative(float xRot, float yRot)
{
	rotateAbsolute(xRot, m_worldUp); //rotate x component
	const float angle = getRotationRadXZ();
	rotateAbsolute(yRot, glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle))); //rotate y component
}