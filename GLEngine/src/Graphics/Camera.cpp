#include "Graphics\Camera.h"

#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\compatibility.hpp>

static const glm::vec3 UP(0, 1, 0);

Camera::~Camera()
{

}

void Camera::initialize(const glm::vec3& position, const glm::vec3& direction, float viewportWidth, float viewportHeight, float near, float far)
{
	m_position = position;
	m_direction = direction;
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;
	m_near = near;
	m_far = far;
}

bool Camera::frustumContains(const glm::vec3* const vertices, unsigned int numVertices) const
{
	for (unsigned int i = 0; i < numVertices; ++i)
	if (m_frustum.pointInFrustum(vertices[i]))
		return true;
	return false;
}

bool Camera::frustumContainsSpheres(const glm::vec3* const vertices, unsigned int numVertices, float sphereRadius) const
{
	for (unsigned int i = 0; i < numVertices; ++i)
	if (m_frustum.sphereInFrustum(vertices[i], sphereRadius))
		return true;
	return false;
}

void Camera::translate(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Camera::translateRelative(float x, float y, float z)
{
	float angle = getRotationRadXZ();
	float xTrans = x * glm::cos(angle) + z * glm::sin(angle);
	float zTrans = z * glm::cos(angle) - x * glm::sin(angle);

	translate(-xTrans, y, -zTrans);
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

void Camera::rotate(float angleRad, const glm::vec3& axis)
{
	glm::vec3 tmp = m_direction;
	m_direction = glm::rotate(m_direction, angleRad, axis);
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

void Camera::rotateRelative(float xRot, float yRot)
{
	//rotate x component
	rotate(xRot, UP);

	//rotate y component
	float angle = getRotationRadXZ();
	rotate(yRot, glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle)));
}

void Camera::update()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_direction, m_up);
	m_combinedMatrix = m_projectionMatrix * m_viewMatrix;

	m_frustum.calculateFrustum(m_combinedMatrix);
}



void Camera::lookAt(float x, float y, float z)
{
	m_direction = glm::normalize(glm::vec3(x - m_position.x, y - m_position.y, z - m_position.z));
}

void Camera::lookAtDir(const glm::vec3& direction)
{
	m_direction = direction;
}

void Camera::resize(int width, int height)
{
	m_viewportWidth = float(width);
	m_viewportHeight = float(height);
}

void Camera::setNearFar(float near, float far)
{
	m_near = near;
	m_far = far;
}