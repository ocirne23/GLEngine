#pragma once

#include "Core.h"

#include "Input/KeyListener.h"
#include "Input/MouseListener.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
enum Key;
enum MouseButton;

/**
Mouse/key listener which moves the given camera like a first person game would.
*/
class FPSCameraController : public MouseListener, public KeyListener
{
public:
	FPSCameraController();
	~FPSCameraController();

	void initialize(PerspectiveCamera& camera, const glm::vec3& startDir);

	void update(float deltaSec);

	void setCameraSpeed(float cameraSpeed);

	virtual bool keyDown(Key key) OVERRIDE;
	virtual bool keyUp(Key key) OVERRIDE;

	virtual bool mouseDown(MouseButton key, int xPos, int yPos) OVERRIDE;
	virtual bool mouseUp(MouseButton key, int xPos, int yPos) OVERRIDE;
	virtual bool mouseMoved(int xPos, int yPos, int xMove, int yMove) OVERRIDE;
	virtual bool mouseScrolled(int amount) OVERRIDE;

private:
	bool m_initialized;
	PerspectiveCamera* m_camera;
	bool m_lmbPressed, m_rmbPressed;
	float m_cameraSpeed;
	float m_diagonalCameraSpeed;

	bool
		m_isWPressed,
		m_isAPressed,
		m_isSPressed,
		m_isDPressed,
		m_isSpacePressed,
		m_isShiftPressed;

	glm::vec3 m_lookDir;
};