#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera() {}
	~PerspectiveCamera() {}

	void initialize(const glm::vec3& position, const glm::vec3& direction, float viewportWidth, float viewportHeight, float horizontalFov, float near, float far);
	void resize(int width, int height) override;
	void setNearFar(float near, float far);
	void setHorizontalFieldOfView(float fovh);

	float getVFov() const { return m_vFieldOfView; }
	float getHFov() const { return m_hFieldOfView; }

private:

	void updateProjection();

private:

	float m_hFieldOfView = 0.0f;
	float m_vFieldOfView = 0.0f;
};