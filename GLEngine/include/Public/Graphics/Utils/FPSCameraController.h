#pragma once

#include "Core.h"
#include "Input/Input.h"
#include <glm/glm.hpp>

class PerspectiveCamera;

class FPSCameraController
{
public:

	FPSCameraController();
	void update(PerspectiveCamera& camera, float deltaSec);
	void setCameraSpeed(float a_cameraSpeed) { m_cameraSpeed = a_cameraSpeed; }
	float getCameraSpeed() const { return m_cameraSpeed; }

private:

	glm::vec3 getLocalSpaceMovementVector();

private:

	Input::MouseMovedListener m_mouseMovedListener;
	Input::KeyDownListener m_keyDownListener;

	int m_mouseXMoveAmount = 0;
	int m_mouseYMoveAmount = 0;
	float m_mouseLookSensitivity = 0.7f;
	float m_cameraSpeed = 1.0f;
};