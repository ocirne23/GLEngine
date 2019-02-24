#include "Graphics\ICamera.h"
#include "Camera.h"

ICamera::ICamera()
	: m_impl(make_pimpl<Camera>())
{
}

void ICamera::initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp)
{
	m_impl->initialize(width, height, horizontalFov, near, far, projection, worldUp);
}

void ICamera::updateMatrices()
{
	m_impl->updateMatrices();
}

void ICamera::setPosition(const glm::vec3& position)
{
	m_impl->setPosition(position);
}

void ICamera::setDirection(const glm::vec3& direction)
{
	m_impl->setDirection(direction);
}

void ICamera::setHorizontalFov(float hFov)
{
	m_impl->setHorizontalFov(hFov);
}

void ICamera::setWorldUp(const glm::vec3& worldUp)
{
	m_impl->setWorldUp(worldUp);
}

void ICamera::lookAtPoint(const glm::vec3& position)
{
	m_impl->lookAtPoint(position);
}

void ICamera::translateAbsolute(const glm::vec3& translation)
{
	m_impl->translateAbsolute(translation);
}

void ICamera::translateRelative(const glm::vec3& translation)
{
	m_impl->translateRelative(translation);
}

void ICamera::rotateAbsolute(float angle, const glm::vec3& axis)
{
	m_impl->rotateAbsolute(angle, axis);
}

void ICamera::rotateRelative(float xRot, float yRot)
{
	m_impl->rotateRelative(xRot, yRot);
}

float ICamera::getRotationRadXY() const
{
	return m_impl->getRotationRadXY();
}

float ICamera::getRotationRadXZ() const
{
	return m_impl->getRotationRadXZ();
}

float ICamera::getRotationRadYZ() const
{
	return m_impl->getRotationRadYZ();
}

const Frustum& ICamera::getFrustum() const
{
	return m_impl->getFrustum();
}

const glm::mat4& ICamera::getCombinedMatrix() const
{
	return m_impl->getCombinedMatrix();
}

const glm::mat4& ICamera::getProjectionMatrix() const
{
	return m_impl->getProjectionMatrix();
}

const glm::mat4& ICamera::getViewMatrix() const
{
	return m_impl->getViewMatrix();
}

const glm::vec3& ICamera::getPosition() const
{
	return m_impl->getPosition();
}

const glm::vec3 & ICamera::getDirection() const
{
	return m_impl->getDirection();
}

const glm::vec3 & ICamera::getUp() const
{
	return m_impl->getUp();
}

float ICamera::getWidth() const
{
	return m_impl->getWidth();
}

float ICamera::getHeight() const
{
	return m_impl->getHeight();
}

float ICamera::getVFov() const
{
	return m_impl->getVFov();
}

float ICamera::getHFov() const
{
	return m_impl->getHFov();
}

float ICamera::getNear() const
{
	return m_impl->getNear();
}

float ICamera::getFar() const
{
	return m_impl->getFar();
}
