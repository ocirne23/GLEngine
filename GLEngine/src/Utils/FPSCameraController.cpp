#include "Utils\FPSCameraController.h"

#include "Graphics\PerspectiveCamera.h"

#include "Input\Key.h"
#include "Input\MouseButton.h"

#include <glm\glm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\compatibility.hpp>

static const float DEFAULT_CAMERA_SPEED = 10.0f;
static const float MOUSE_LOOK_SENSITIVITY = 0.7f;
static const glm::vec3 UP(0, 1, 0);

FPSCameraController::FPSCameraController()
: m_lmbPressed(false)
, m_rmbPressed(false)
, m_isWPressed(false), m_isAPressed(false), m_isSPressed(false), m_isDPressed(false)
, m_isSpacePressed(false), m_isShiftPressed(false)
{
	setCameraSpeed(DEFAULT_CAMERA_SPEED);
}

FPSCameraController::~FPSCameraController()
{
}

void FPSCameraController::initialize(PerspectiveCamera& camera, const glm::vec3& lookDir)
{
	m_camera = &camera;
	m_lookDir = lookDir;
	m_initialized = true;
}

void FPSCameraController::setCameraSpeed(float cameraSpeed)
{
	m_cameraSpeed = cameraSpeed;
	m_diagonalCameraSpeed = glm::sqrt((cameraSpeed * cameraSpeed) * 0.5f);
}

void FPSCameraController::update(float deltaSec)
{
	if (!m_initialized)
		return;

	bool w = m_isWPressed;
	bool a = m_isAPressed;
	bool s = m_isSPressed;
	bool d = m_isDPressed;
	bool space = m_isSpacePressed;
	bool shift = m_isShiftPressed;

	if (w && s)
		w = false, s = false;
	if (a && d)
		a = false, d = false;
	if (space && shift)
		space = false, shift = false;

	if (w)
	{
		if (a)
			m_camera->translateRelative(-m_diagonalCameraSpeed * deltaSec, 0.0f, -m_diagonalCameraSpeed * deltaSec);
		else if (d)
			m_camera->translateRelative(m_diagonalCameraSpeed * deltaSec, 0.0f, -m_diagonalCameraSpeed * deltaSec);
		else
			m_camera->translateRelative(0.0f, 0.0f, -m_cameraSpeed * deltaSec);
	}
	else if (s)
	{
		if (a)
			m_camera->translateRelative(-m_diagonalCameraSpeed * deltaSec, 0.0f, m_diagonalCameraSpeed * deltaSec);
		else if (d)
			m_camera->translateRelative(m_diagonalCameraSpeed * deltaSec, 0.0f, m_diagonalCameraSpeed * deltaSec);
		else
			m_camera->translateRelative(0.0f, 0.0f, m_cameraSpeed * deltaSec);
	}
	else if (a)
		m_camera->translateRelative(-m_cameraSpeed * deltaSec, 0.0f, 0.0f);
	else if (d)
		m_camera->translateRelative(m_cameraSpeed * deltaSec, 0.0f, 0.0f);

	if (space)
		m_camera->translateRelative(0.0f, m_cameraSpeed * deltaSec, 0.0f);
	else if (shift)
		m_camera->translateRelative(0.0f, -m_cameraSpeed * deltaSec, 0.0f);

	m_camera->lookAtDir(m_lookDir);
}

bool FPSCameraController::mouseDown(MouseButton key, int xPos, int yPos)
{
	if (key == 1)
		m_lmbPressed = true;
	if (key == 3)
		m_rmbPressed = true;
	return false;
}

bool FPSCameraController::mouseUp(MouseButton key, int xPos, int yPos)
{
	if (key == 1)
		m_lmbPressed = false;
	if (key == 3)
		m_rmbPressed = false;
	return false;
}

bool FPSCameraController::mouseMoved(int xPos, int yPos, int xMove, int yMove)
{
	//rotate horizontally
	m_lookDir = glm::rotate(m_lookDir, -xMove * MOUSE_LOOK_SENSITIVITY, UP);

	//calculate axis to rotate vertically on
	float xzAngle = glm::atan2(m_lookDir.x, m_lookDir.z);

	glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));

	//rotate vertically
	glm::vec3 tmp = m_lookDir;
	m_lookDir = glm::rotate(m_lookDir, -yMove * MOUSE_LOOK_SENSITIVITY, yRotAxis);
	//limit vertical look movement

	if (m_lookDir.y > 0.99f || m_lookDir.y < -0.99f)
		m_lookDir = tmp;

	return false;
}

bool FPSCameraController::mouseScrolled(int amount)
{
	return false;
}

bool FPSCameraController::keyDown(Key key)
{
	switch (key)
	{
	case Key_W:
		m_isWPressed = true;
		break;
	case Key_A:
		m_isAPressed = true;
		break;
	case Key_S:
		m_isSPressed = true;
		break;
	case Key_D:
		m_isDPressed = true;
		break;
	case Key_SPACE:
		m_isSpacePressed = true;
		break;
	case Key_LSHIFT:
		m_isShiftPressed = true;
		break;
	}
	return false;
}

bool FPSCameraController::keyUp(Key key)
{
	switch (key)
	{
	case Key_W:
		m_isWPressed = false;
		break;
	case Key_A:
		m_isAPressed = false;
		break;
	case Key_S:
		m_isSPressed = false;
		break;
	case Key_D:
		m_isDPressed = false;
		break;
	case Key_SPACE:
		m_isSpacePressed = false;
		break;
	case Key_LSHIFT:
		m_isShiftPressed = false;
		break;
	}
	return false;
}
