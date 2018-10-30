#pragma once

#include "Graphics/ICamera.h"
#include "Frustum.h"

class Camera : public ICamera
{
public:

	void initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp);
	void updateMatrices();

	void setPosition(const glm::vec3& position);
	void setDirection(const glm::vec3& direction);
	void setHorizontalFov(float hFov);
	void setWorldUp(const glm::vec3& worldUp);
	void lookAtPoint(const glm::vec3& position);

	void translateAbsolute(const glm::vec3& translation);
	void translateRelative(const glm::vec3& translation);
	void rotateAbsolute(float angle, const glm::vec3& axis);
	void rotateRelative(float xRot, float yRot);

	const Frustum& getFrustum() const            { return m_frustum; }
	float getRotationRadXY() const               { return std::atan2(m_direction.x, m_direction.y); }
	float getRotationRadXZ() const               { return std::atan2(m_direction.x, m_direction.z); }
	float getRotationRadYZ() const               { return std::atan2(m_direction.y, m_direction.z); }
	const glm::mat4& getCombinedMatrix() const   { return m_combinedMatrix; }
	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
	const glm::mat4& getViewMatrix() const       { return m_viewMatrix; }
	const glm::vec3& getPosition() const         { return m_position; }
	const glm::vec3& getDirection() const        { return m_direction; }
	const glm::vec3& getUp() const               { return m_up; }
	float getWidth() const                       { return m_width; }
	float getHeight() const                      { return m_height; }
	float getVFov() const                        { return m_vFieldOfView; }
	float getHFov() const                        { return m_hFieldOfView; }
	float getNear() const                        { return m_near; }
	float getFar() const                         { return m_far; }

private:

	EProjection m_projection;
	Frustum m_frustum;

	glm::mat4 m_combinedMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_direction = glm::vec3(0, 0, -1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);
	glm::vec3 m_worldUp = glm::vec3(0, 1, 0);

	float m_width = 0.0f;
	float m_height = 0.0f;
	float m_near = 0.0f;
	float m_far = 0.0f;
	float m_hFieldOfView = 0.0f;
	float m_vFieldOfView = 0.0f;
};