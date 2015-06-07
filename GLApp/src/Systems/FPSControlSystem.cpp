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
	m_mouseMovedListener.setFunc([this](uint xPos, uint yPos, int deltaX, int deltaY)
	{
		if (GLEngine::input->isMousePressed(EMouseButton::LEFT))
		{
			m_xMoveAmount += deltaX;
			m_yMoveAmount += deltaY;
		}
	});

	m_windowQuitListener.setFunc([this]() {
		GLEngine::shutdown();
	});
}

FPSControlSystem::~FPSControlSystem()
{
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
	moveAmount *= a_dt;

	entityx::ComponentHandle<FPSControlledComponent> control;
	entityx::ComponentHandle<TransformComponent> transform;
	for (entityx::Entity entity : a_entities.entities_with_components(control, transform))
	{
		glm::vec3 position(transform->transform[3]);
		glm::mat3 rotation(transform->transform);
		glm::vec3 direction(FORWARD * rotation);

		// Rotate horizontally
		direction = glm::rotate(direction, -m_xMoveAmount * control->lookSensitivity, UP); 
		float xzAngle = glm::atan2(direction.x, direction.z); //calculate axis to rotate vertically on

		// Rotate vertically
		glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));
		glm::vec3 tmp = direction;
		direction = glm::rotate(direction, -m_yMoveAmount * control->lookSensitivity, yRotAxis); 

		// Limit vertical look movement
		if (direction.y > 0.99f || direction.y < -0.99f)
			direction = tmp;

		float xTrans = moveAmount.x * glm::cos(xzAngle) + moveAmount.z * glm::sin(xzAngle);
		float zTrans = moveAmount.z * glm::cos(xzAngle) - moveAmount.x * glm::sin(xzAngle);

		position.x += -xTrans * control->moveSpeed;
		position.z += -zTrans * control->moveSpeed;
		position.y += moveAmount.y * control->moveSpeed;

		transform->transform = glm::lookAt(position, position + direction, UP);
		transform->transform[3] = glm::vec4(position, 1.0);
	}
	m_xMoveAmount = 0;
	m_yMoveAmount = 0;
}