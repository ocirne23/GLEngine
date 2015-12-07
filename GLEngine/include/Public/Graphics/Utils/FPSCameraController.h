#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
enum class EKey;
enum class EMouseButton;

class FPSCameraController
{
public:
	FPSCameraController();

	void initialize(PerspectiveCamera& camera);
	void update(float deltaSec);
	void setCameraSpeed(float cameraSpeed);
	float getCameraSpeed() const { return m_cameraSpeed; }

	bool keyDown(EKey key);
	bool keyUp(EKey key);

	bool mouseDown(EMouseButton key, int xPos, int yPos);
	bool mouseUp(EMouseButton key, int xPos, int yPos);
	bool mouseMoved(uint xPos, uint yPos, int xMove, int yMove);
	bool mouseScrolled(int amount);

private:

	bool m_initialized          = false;
	PerspectiveCamera* m_camera = NULL;
	float m_cameraSpeed         = 0.0f;
	float m_diagonalCameraSpeed = 0.0f;

	bool m_lmbPressed     = false;
	bool m_rmbPressed     = false;
	bool m_isWPressed     = false;
	bool m_isAPressed     = false;
	bool m_isSPressed     = false;
	bool m_isDPressed     = false;
	bool m_isSpacePressed = false;
	bool m_isShiftPressed = false;

	glm::vec3 m_lookDir;
};