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
static const float MOUSE_LOOK_SENSITIVITY = 0.7f;

END_UNNAMED_NAMESPACE()

FPSControlSystem::FPSControlSystem()
{
	m_mouseMovedListener.setFunc([this](uint a_xPos, uint a_yPos, int a_deltaX, int a_deltaY)
	{
		if (GLEngine::input->isMousePressed(EMouseButton::LEFT))
		{
			m_mouseXMoveAmount += a_deltaX;
			m_mouseYMoveAmount += a_deltaY;
		}
	});
}

void FPSControlSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	glm::vec3 movement = getLocalSpaceMovementVector();

	entityx::ComponentHandle<FPSControlledComponent> controlComponent;
	entityx::ComponentHandle<TransformComponent> transformComponent;
	for (entityx::Entity entity : a_entities.entities_with_components(controlComponent, transformComponent))
	{
		float xLookRotation = -m_mouseXMoveAmount * controlComponent->lookSensitivity;
		float yLookRotation = -m_mouseYMoveAmount * controlComponent->lookSensitivity;

		glm::vec3 position(transformComponent->transform[3]);
		glm::mat3 rotation(transformComponent->transform);
		glm::vec3 direction(FORWARD * rotation);
		
		// Rotate horizontally
		direction = glm::rotate(direction, xLookRotation, UP); 
		float xzAngle = glm::atan2(direction.x, direction.z); //calculate axis to rotate vertically on

		// Rotate vertically
		glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));
		glm::vec3 tmp = direction;
		direction = glm::rotate(direction, yLookRotation, yRotAxis); 

		// Limit vertical look movement
		if (direction.y > 0.99f || direction.y < -0.99f)
			direction = tmp;

		// Rotate xz movement based on direction
		float xTrans = movement.x * glm::cos(xzAngle) + movement.z * glm::sin(xzAngle);
		float zTrans = movement.z * glm::cos(xzAngle) - movement.x * glm::sin(xzAngle);

		position.x += -xTrans * controlComponent->moveSpeed * a_dt;
		position.z += -zTrans * controlComponent->moveSpeed * a_dt;
		position.y += movement.y * controlComponent->moveSpeed * a_dt;

		transformComponent->transform = glm::lookAt(position, position + direction, UP);
		transformComponent->transform[3] = glm::vec4(position, 1.0);
	}
	m_mouseXMoveAmount = 0;
	m_mouseYMoveAmount = 0;
}

glm::vec3 FPSControlSystem::getLocalSpaceMovementVector()
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
