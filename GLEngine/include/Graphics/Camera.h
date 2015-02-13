#pragma once

#include "Core.h"

#include "Graphics/Frustum.h"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera() {}
	virtual ~Camera() {}

	void initialize(const glm::vec3& position, const glm::vec3& direction, float viewportWidth, float viewportHeight, float near, float a_far);

	virtual void resize(int width, int height);
	virtual void update();
	virtual void setNearFar(float near, float a_far);

	void setPosition(float x, float y, float z);
	void setPosition(const glm::vec3& position);
	void translate(float x, float y, float z);
	/** Translates the camera position relative to the view of the camera */
	void translateRelative(float x, float y, float z);
	void translateRelative(const glm::vec3& trans);
	void rotate(float angle, const glm::vec3& axis);
	void rotateRelative(float xRot, float yRot);
	void lookAt(float x, float y, float z);
	void lookAtDir(const glm::vec3& dir);

	bool frustumContains(const glm::vec3* const vertices, uint numVertices) const;
	bool frustumContainsSpheres(const glm::vec3* const spherePositions, uint numSpheres, float sphereRadius) const;

	float getRotationRadXY();
	float getRotationRadXZ();
	float getRotationRadYZ();

public:

	glm::mat4 m_combinedMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_up;

	Frustum m_frustum;

	float m_viewportWidth	= 0.0f;
	float m_viewportHeight	= 0.0f;
	float m_near			= 0.0f;
	float m_far				= 0.0f;
};