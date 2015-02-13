#include "Graphics/Camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>

BEGIN_UNNAMED_NAMESPACE()
static const glm::vec3 UP(0, 1, 0);
END_UNNAMED_NAMESPACE()

void Camera::initialize(const glm::vec3& a_position, const glm::vec3& a_direction, float a_viewportWidth, float a_viewportHeight, float a_near, float a_far)
{
	m_position = a_position;
	m_direction = a_direction;
	m_viewportWidth = a_viewportWidth;
	m_viewportHeight = a_viewportHeight;
	m_near = a_near;
	m_far = a_far;
	m_up = UP;
}

bool Camera::frustumContains(const glm::vec3* const a_vertices, uint a_numVertices) const
{
	for (uint i = 0; i < a_numVertices; ++i)
		if (m_frustum.pointInFrustum(a_vertices[i]))
			return true;
	return false;
}

bool Camera::frustumContainsSpheres(const glm::vec3* const a_vertices, uint a_numVertices, float a_sphereRadius) const
{
	for (uint i = 0; i < a_numVertices; ++i)
		if (m_frustum.sphereInFrustum(a_vertices[i], a_sphereRadius))
			return true;
	return false;
}

void Camera::setPosition(const glm::vec3& a_position)
{
	m_position = a_position;
}

void Camera::setPosition(float a_x, float a_y, float a_z)
{
	m_position.x = a_x;
	m_position.y = a_y;
	m_position.z = a_z;
}

void Camera::translate(float a_x, float a_y, float a_z)
{
	m_position.x += a_x;
	m_position.y += a_y;
	m_position.z += a_z;
}

void Camera::translateRelative(float a_x, float a_y, float a_z)
{
	float angle = getRotationRadXZ();
	float xTrans = a_x * glm::cos(angle) + a_z * glm::sin(angle);
	float zTrans = a_z * glm::cos(angle) - a_x * glm::sin(angle);

	translate(-xTrans, a_y, -zTrans);
}

void Camera::translateRelative(const glm::vec3& a_trans)
{
	translateRelative(a_trans.x, a_trans.y, a_trans.z);
}

float Camera::getRotationRadXY()
{
	return glm::atan2(m_direction.x, m_direction.y);
}

float Camera::getRotationRadXZ()
{
	return glm::atan2(m_direction.x, m_direction.z);
}

float Camera::getRotationRadYZ()
{
	return glm::atan2(m_direction.y, m_direction.z);
}

void Camera::rotate(float a_angleRad, const glm::vec3& a_axis)
{
	glm::vec3 tmp = m_direction;
	m_direction = glm::rotate(m_direction, a_angleRad, a_axis);
	//limit vertical look movement
	if (m_direction.y > 0.99f || m_direction.y < -0.99f)
	{
		m_direction = tmp;
		return;
	}
	//recalculate up vector.
	glm::vec3 side = glm::cross(m_direction, UP);
	m_up = glm::cross(side, m_direction);
}

void Camera::rotateRelative(float a_xRot, float a_yRot)
{
	rotate(a_xRot, UP); //rotate x component
	float angle = getRotationRadXZ();
	rotate(a_yRot, glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle))); //rotate y component
}

void Camera::update()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_direction, m_up);
	m_combinedMatrix = m_projectionMatrix * m_viewMatrix;
	m_frustum.calculateFrustum(m_combinedMatrix);
}

void Camera::lookAt(float a_x, float a_y, float a_z)
{
	m_direction = glm::normalize(glm::vec3(a_x - m_position.x, a_y - m_position.y, a_z - m_position.z));
}

void Camera::lookAtDir(const glm::vec3& a_direction)
{
	m_direction = a_direction;
}

void Camera::resize(int a_width, int a_height)
{
	m_viewportWidth = float(a_width);
	m_viewportHeight = float(a_height);
}

void Camera::setNearFar(float a_near, float a_far)
{
	m_near = a_near;
	m_far = a_far;
}