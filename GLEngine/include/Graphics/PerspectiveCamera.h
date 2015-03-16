#pragma once

#include "Core.h"

#include "Graphics/Frustum.h"

#include <glm/glm.hpp>

class PerspectiveCamera
{
public:
	PerspectiveCamera() {}
	~PerspectiveCamera() {}

	void initialize(float viewportWidth, float viewportHeight, float horizontalFov, float near, float far);

	void updateMatrices();

	void resize(int width, int height);
	void setNearFar(float near, float a_far);

	void setPosition(const glm::vec3& position);
	void translate(const glm::vec3& translation);
	void translateRelative(const glm::vec3& translation);
	void lookAtPoint(const glm::vec3& point);
	void lookAtDir(const glm::vec3& dir);

	void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }
	void translate(float x, float y, float z) { translate(glm::vec3(x, y, z)); }
	void translateRelative(float x, float y, float z) { translateRelative(glm::vec3(x, y, z)); }
	void lookAtPoint(float x, float y, float z) { lookAtPoint(glm::vec3(x, y, z)); }
	void lookAtDir(float x, float y, float z) { lookAtDir(glm::vec3(x, y, z)); }

	void rotate(float angle, const glm::vec3& axis);
	void rotateRelative(float xRot, float yRot);
	void setHorizontalFieldOfView(float fovh);

	float getRotationRadXY();
	float getRotationRadXZ();
	float getRotationRadYZ();

	const Frustum& getFrustum()	const { return m_frustum; }
	const glm::mat4& getCombinedMatrix() const { return m_combinedMatrix; }
	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
	const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getDirection() const { return m_direction; }
	const glm::vec3& getUp() const { return m_up; }
	float getVFov() const { return m_vFieldOfView; }
	float getHFov() const { return m_hFieldOfView; }
	float getNear() const { return m_near; }
	float getFar() const { return m_far; }

private:

	Frustum m_frustum;

	glm::mat4 m_combinedMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_direction = glm::vec3(0, 0, -1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);

	float m_viewportWidth = 0.0f;
	float m_viewportHeight = 0.0f;
	float m_near = 0.0f;
	float m_far = 0.0f;
	float m_hFieldOfView = 0.0f;
	float m_vFieldOfView = 0.0f;
};