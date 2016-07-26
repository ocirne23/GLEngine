#pragma once

#include "Core.h"
#include <glm/glm.hpp>

class PerspectiveCamera;

class FPSCameraController
{
public:

	void update(PerspectiveCamera& camera, float deltaSec, bool focused);
	void setCameraSpeed(float a_cameraSpeed) { m_cameraSpeed = a_cameraSpeed; }
	float getCameraSpeed() const { return m_cameraSpeed; }

private:

	glm::ivec2 m_mousePos = glm::ivec2(-1, -1);
	float m_pitchDeg = 0.0f;
	float m_yawDeg = 0.0f;
	float m_mouseLookSensitivity = 1.0f;
	float m_cameraSpeed = 1.0f;
	float m_verticalLookAngle = 0.0f;
};