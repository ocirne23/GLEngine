#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
enum Key;
enum MouseButton;

class FPSCameraController
{
public:
	FPSCameraController();
	~FPSCameraController();

	void initialize(PerspectiveCamera& camera, const glm::vec3& startDir);
	void update(float deltaSec);
	void setCameraSpeed(float cameraSpeed);

private:

	bool keyDown(Key key);
	bool keyUp(Key key);

	bool mouseDown(MouseButton key, int xPos, int yPos);
	bool mouseUp(MouseButton key, int xPos, int yPos);
	bool mouseMoved(int xPos, int yPos, int xMove, int yMove);
	bool mouseScrolled(int amount);

private:

	bool m_initialized = false;
	PerspectiveCamera* m_camera = NULL;
	float m_cameraSpeed = 0.0f;
	float m_diagonalCameraSpeed = 0.0f;

	bool m_lmbPressed = false;
	bool m_rmbPressed = false;
	bool m_isWPressed = false;
	bool m_isAPressed = false;
	bool m_isSPressed = false;
	bool m_isDPressed = false;
	bool m_isSpacePressed = false;
	bool m_isShiftPressed = false;

	glm::vec3 m_lookDir;
};