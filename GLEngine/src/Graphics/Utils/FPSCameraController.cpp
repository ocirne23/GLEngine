#include "Graphics/Utils/FPSCameraController.h"

#include "Input/Input.h"
#include "Graphics/Utils/PerspectiveCamera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const glm::vec3 UP(0, 1, 0);
static const glm::vec3 FORWARD(0, 0, -1);

glm::vec3 getLocalSpaceMovementVector()
{
	static const float DIAGONAL_1F = glm::sqrt(0.5f);

	Input& input = *GLEngine::input;
	glm::vec3 movementVector(0);
	if (input.isKeyPressed(EKey::W))
		if (input.isKeyPressed(EKey::A))      movementVector = glm::vec3(-DIAGONAL_1F, 0, -DIAGONAL_1F);
		else if (input.isKeyPressed(EKey::D)) movementVector = glm::vec3(DIAGONAL_1F, 0, -DIAGONAL_1F);
		else                                  movementVector = glm::vec3(0, 0, -1.0f);
	else if (input.isKeyPressed(EKey::S))
		if (input.isKeyPressed(EKey::A))      movementVector = glm::vec3(-DIAGONAL_1F, 0, DIAGONAL_1F);
		else if (input.isKeyPressed(EKey::D)) movementVector = glm::vec3(DIAGONAL_1F, 0, DIAGONAL_1F);
		else                                  movementVector = glm::vec3(0, 0, 1.0f);
	else if (input.isKeyPressed(EKey::A))     movementVector = glm::vec3(-1.0f, 0, 0);
	else if (input.isKeyPressed(EKey::D))     movementVector = glm::vec3(1.0f, 0, 0);
	if (input.isKeyPressed(EKey::SPACE))      movementVector.y += 1.0f;
	if (input.isKeyPressed(EKey::LSHIFT))     movementVector.y -= 1.0f;

	return movementVector;
}

END_UNNAMED_NAMESPACE()

void FPSCameraController::update(PerspectiveCamera& a_camera, float a_deltaSec, bool a_focused)
{
	glm::ivec2 newMousePos;
	GLEngine::input->getMousePosition(newMousePos.x, newMousePos.y);
	if (m_mousePos.x == -1 && m_mousePos.y == -1) // First update, dont process movement
		m_mousePos = newMousePos;
	glm::ivec2 mouseMovement = m_mousePos - newMousePos;
	m_mousePos = newMousePos;

	if (!a_focused)
		return;
	if (!GLEngine::input->isMousePressed(EMouseButton::LEFT))
		mouseMovement = glm::ivec2(0);

	const float xLookRotation = (mouseMovement.x * m_mouseLookSensitivity);
	const float yLookRotation = (mouseMovement.y * m_mouseLookSensitivity);

	glm::vec3 direction = a_camera.getDirection();
	const glm::vec3 tmp = direction;
	const glm::vec3 side = glm::cross(direction, UP);
	const glm::vec3 up = glm::cross(side, direction);
	const glm::quat yaw = glm::angleAxis(glm::radians(xLookRotation), up);

	const float xzAngle = std::atan2(direction.x, direction.z); //calculate axis to rotate vertically on
	const glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));
	const glm::quat pitch = glm::angleAxis(glm::radians(yLookRotation), yRotAxis);
	const glm::quat orientation = glm::normalize(glm::cross(pitch, yaw));
	direction = glm::rotate(orientation, direction);
	
	const float newXZ = std::atan2(direction.x, direction.z);
	const float xzDiff = glm::abs(xzAngle - newXZ);
	if (xzDiff > 2.0f && xzDiff < 4.0f) // flip protection
	{
		direction = tmp;
	}

	a_camera.lookAtDir(direction);
	a_camera.translateRelative(getLocalSpaceMovementVector() * m_cameraSpeed * a_deltaSec);
	a_camera.updateMatrices();
}