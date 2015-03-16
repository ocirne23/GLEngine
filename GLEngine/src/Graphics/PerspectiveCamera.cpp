#include "Graphics/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const glm::vec3 UP(0, 1, 0);

END_UNNAMED_NAMESPACE()

void PerspectiveCamera::initialize(float a_viewportWidth, float a_viewportHeight, float a_horizontalFov, float a_near, float a_far)
{
    m_near = a_near;
    m_far = a_far;
    m_viewportWidth = a_viewportWidth;
    m_viewportHeight = a_viewportHeight;
    setHorizontalFieldOfView(a_horizontalFov);
    updateMatrices();
}

void PerspectiveCamera::setPosition(const glm::vec3& a_position)
{
    m_position = a_position;
}

void PerspectiveCamera::translate(const glm::vec3& a_translation)
{
    m_position += a_translation;
}

void PerspectiveCamera::translateRelative(const glm::vec3& a_trans)
{
    float angle = getRotationRadXZ();
    float xTrans = a_trans.x * glm::cos(angle) + a_trans.z * glm::sin(angle);
    float zTrans = a_trans.z * glm::cos(angle) - a_trans.x * glm::sin(angle);

    translate(-xTrans, a_trans.y, -zTrans);
}

float PerspectiveCamera::getRotationRadXY()
{
    return glm::atan2(m_direction.x, m_direction.y);
}

float PerspectiveCamera::getRotationRadXZ()
{
    return glm::atan2(m_direction.x, m_direction.z);
}

float PerspectiveCamera::getRotationRadYZ()
{
    return glm::atan2(m_direction.y, m_direction.z);
}

void PerspectiveCamera::rotate(float a_angleRad, const glm::vec3& a_axis)
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

void PerspectiveCamera::rotateRelative(float a_xRot, float a_yRot)
{
    rotate(a_xRot, UP); //rotate x component
    float angle = getRotationRadXZ();
    rotate(a_yRot, glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle))); //rotate y component
}

void PerspectiveCamera::updateMatrices()
{
    m_projectionMatrix = glm::perspective(m_vFieldOfView, m_viewportWidth / m_viewportHeight, m_near, m_far);
    m_viewMatrix = glm::lookAt(m_position, m_position + m_direction, m_up);
    m_combinedMatrix = m_projectionMatrix * m_viewMatrix;
    m_frustum.calculateFrustum(m_combinedMatrix);
}

void PerspectiveCamera::lookAtPoint(const glm::vec3& a_point)
{
    m_direction = glm::normalize(a_point - m_position);
}

void PerspectiveCamera::lookAtDir(const glm::vec3& a_direction)
{
    m_direction = a_direction;
}

void PerspectiveCamera::setHorizontalFieldOfView(float a_fovh)
{
    m_hFieldOfView = a_fovh;
    m_vFieldOfView = glm::degrees(glm::atan(glm::tan(glm::radians(m_hFieldOfView) * (m_viewportHeight / m_viewportWidth))));
}

void PerspectiveCamera::resize(int a_width, int a_height)
{
    m_viewportWidth = (float) a_width;
    m_viewportHeight = (float) a_height;
}

void PerspectiveCamera::setNearFar(float a_near, float a_far)
{
    m_near = a_near;
    m_far = a_far;
}