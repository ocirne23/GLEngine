#pragma once

#include "Core.h"

#include "Input/KeyListener.h"
#include "Input/MouseListener.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
enum Key;
enum MouseButton;

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

	bool m_initialized			= false;
	PerspectiveCamera* m_camera	= NULL;
	float m_cameraSpeed			= 0.0f;
	float m_diagonalCameraSpeed	= 0.0f;

	bool m_lmbPressed		= false;
	bool m_rmbPressed		= false;
	bool m_isWPressed		= false;
	bool m_isAPressed		= false;
	bool m_isSPressed		= false;
	bool m_isDPressed		= false;
	bool m_isSpacePressed	= false;
	bool m_isShiftPressed	= false;

	glm::vec3 m_lookDir;
};