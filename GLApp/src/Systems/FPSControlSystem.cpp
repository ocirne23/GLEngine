#include "Systems/FPSControlSystem.h"

#include "Components/FPSControlledComponent.h"
#include "Components/TransformComponent.h"
#include "GLEngine.h"
#include "Input/Input.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const glm::vec3 UP(0, 1, 0);
static const glm::vec3 FORWARD(0, 0, -1);

static const float MOUSE_LOOK_SENSITIVITY     = 0.7f;
static const float BASE_CAMERA_SPEED          = 1.0f;
static const float BASE_DIAGONAL_CAMERA_SPEED = glm::sqrt(0.5f);

END_UNNAMED_NAMESPACE()

FPSControlSystem::FPSControlSystem()
{
	m_mouseMovedListener.setFunc([this](uint a_xPos, uint a_yPos, int a_deltaX, int a_deltaY)
	{
		if (GLEngine::input->isMousePressed(EMouseButton::LEFT))
		{
			m_xMoveAmount += a_deltaX;
			m_yMoveAmount += a_deltaY;
		}
	});

	m_keyDownListener.setFunc([this](EKey a_key)
	{
		const float speedMod = 0.1f;
		switch (a_key)
		{
		case EKey::KP_PLUS:  m_speedMultiplier *= 1.0f + speedMod; break;
		case EKey::KP_MINUS: m_speedMultiplier *= 1.0f - speedMod; break;
		}
	});
}

void FPSControlSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	Input& input = *GLEngine::input;
	glm::vec3 moveAmount(0);
	if (input.isKeyPressed(EKey::W))
		if (input.isKeyPressed(EKey::A))      moveAmount = glm::vec3(-BASE_DIAGONAL_CAMERA_SPEED, 0, -BASE_DIAGONAL_CAMERA_SPEED);
		else if (input.isKeyPressed(EKey::D)) moveAmount = glm::vec3(BASE_DIAGONAL_CAMERA_SPEED, 0, -BASE_DIAGONAL_CAMERA_SPEED);
		else                                  moveAmount = glm::vec3(0, 0, -BASE_CAMERA_SPEED);
	else if (input.isKeyPressed(EKey::S))
		if (input.isKeyPressed(EKey::A))      moveAmount = glm::vec3(-BASE_DIAGONAL_CAMERA_SPEED, 0, BASE_DIAGONAL_CAMERA_SPEED);
		else if (input.isKeyPressed(EKey::D)) moveAmount = glm::vec3(BASE_DIAGONAL_CAMERA_SPEED, 0, BASE_DIAGONAL_CAMERA_SPEED);
		else                                  moveAmount = glm::vec3(0, 0, BASE_CAMERA_SPEED);
	else if (input.isKeyPressed(EKey::A))     moveAmount = glm::vec3(-BASE_CAMERA_SPEED, 0, 0);
	else if (input.isKeyPressed(EKey::D))     moveAmount = glm::vec3(BASE_CAMERA_SPEED, 0, 0);
	if (input.isKeyPressed(EKey::SPACE))      moveAmount.y += BASE_CAMERA_SPEED;
	if (input.isKeyPressed(EKey::LSHIFT))     moveAmount.y -= BASE_CAMERA_SPEED;
	moveAmount *= a_dt * m_speedMultiplier;

	entityx::ComponentHandle<FPSControlledComponent> controlComponent;
	entityx::ComponentHandle<TransformComponent> transformComponent;
	for (entityx::Entity entity : a_entities.entities_with_components(controlComponent, transformComponent))
	{
		glm::vec3 position(transformComponent->transform[3]);
		glm::mat3 rotation(transformComponent->transform);
		glm::vec3 direction(FORWARD * rotation);

		// Rotate horizontally
		direction = glm::rotate(direction, -m_xMoveAmount * controlComponent->lookSensitivity, UP); 
		float xzAngle = glm::atan2(direction.x, direction.z); //calculate axis to rotate vertically on

		// Rotate vertically
		glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));
		glm::vec3 tmp = direction;
		direction = glm::rotate(direction, -m_yMoveAmount * controlComponent->lookSensitivity, yRotAxis); 

		// Limit vertical look movement
		if (direction.y > 0.99f || direction.y < -0.99f)
			direction = tmp;

		float xTrans = moveAmount.x * glm::cos(xzAngle) + moveAmount.z * glm::sin(xzAngle);
		float zTrans = moveAmount.z * glm::cos(xzAngle) - moveAmount.x * glm::sin(xzAngle);

		position.x += -xTrans * controlComponent->moveSpeed;
		position.z += -zTrans * controlComponent->moveSpeed;
		position.y += moveAmount.y * controlComponent->moveSpeed;

		transformComponent->transform = glm::lookAt(position, position + direction, UP);
		transformComponent->transform[3] = glm::vec4(position, 1.0);
	}
	m_xMoveAmount = 0;
	m_yMoveAmount = 0;
}