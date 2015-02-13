#include "Systems/CameraSystem.h"

#include "Components/CameraComponent.h"
#include "GLEngine.h"
#include "Graphics/PerspectiveCamera.h"
#include "Input/Input.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const glm::vec3 UP(0, 1, 0);

static const float DEFAULT_CAMERA_SPEED = 10.0f;
static const float DIAGONAL_CAMERA_SPEED = glm::sqrt((DEFAULT_CAMERA_SPEED * DEFAULT_CAMERA_SPEED) * 0.5f);
static const float MOUSE_LOOK_SENSITIVITY = 0.7f;

END_UNNAMED_NAMESPACE()

CameraSystem::CameraSystem() : m_lookDir(0.0f, 0.0f, 1.0f)
{
	GLEngine::input->registerMouseListener(this);
}

CameraSystem::~CameraSystem()
{
	GLEngine::input->unregisterMouseListener(this);
}

void CameraSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
	a_eventManager.subscribe<entityx::ComponentRemovedEvent<CameraComponent>>(*this);
}

void CameraSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
	assert(!m_activeCamera);
	m_activeCamera = a_cameraComponentAddedEvent.component->camera;
}

void CameraSystem::receive(const entityx::ComponentRemovedEvent<CameraComponent>& a_cameraComponentRemovedEvent)
{
	assert(m_activeCamera == a_cameraComponentRemovedEvent.component->camera);
	m_activeCamera = NULL;
}

void CameraSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	if (!m_activeCamera)
		return;
	Input& input = *GLEngine::input;
	glm::vec3 moveDir(0);
	if (input.isKeyPressed(Key_W))
		if (input.isKeyPressed(Key_A))		moveDir = glm::vec3(-DIAGONAL_CAMERA_SPEED, 0, -DIAGONAL_CAMERA_SPEED);
		else if (input.isKeyPressed(Key_D)) moveDir = glm::vec3(DIAGONAL_CAMERA_SPEED, 0, -DIAGONAL_CAMERA_SPEED);
		else								moveDir = glm::vec3(0, 0, -DEFAULT_CAMERA_SPEED);
	else if (input.isKeyPressed(Key_S))
		if (input.isKeyPressed(Key_A))		moveDir = glm::vec3(-DIAGONAL_CAMERA_SPEED, 0, DIAGONAL_CAMERA_SPEED);
		else if (input.isKeyPressed(Key_D)) moveDir = glm::vec3(DIAGONAL_CAMERA_SPEED, 0, DIAGONAL_CAMERA_SPEED);
		else								moveDir = glm::vec3(0, 0, DEFAULT_CAMERA_SPEED);
	else if (input.isKeyPressed(Key_A))		moveDir = glm::vec3(-DEFAULT_CAMERA_SPEED, 0, 0);
	else if (input.isKeyPressed(Key_D))		moveDir = glm::vec3(DEFAULT_CAMERA_SPEED, 0, 0);
	
	if (input.isKeyPressed(Key_SPACE))		moveDir.y += DEFAULT_CAMERA_SPEED;
	if (input.isKeyPressed(Key_LSHIFT))		moveDir.y -= DEFAULT_CAMERA_SPEED;

	m_activeCamera->translateRelative(moveDir * dt);
	m_activeCamera->lookAtDir(m_lookDir);
	m_activeCamera->update();
}

bool CameraSystem::mouseMoved(int xPos, int yPos, int xMove, int yMove) 
{
	if (GLEngine::input->isMousePressed(MouseButton_LEFT))
	{
		m_lookDir = glm::rotate(m_lookDir, -xMove * MOUSE_LOOK_SENSITIVITY, UP); //rotate horizontally
		float xzAngle = glm::atan2(m_lookDir.x, m_lookDir.z); //calculate axis to rotate vertically on

		glm::vec3 yRotAxis(-glm::cos(xzAngle), 0.0f, glm::sin(xzAngle));
		glm::vec3 tmp = m_lookDir;
		m_lookDir = glm::rotate(m_lookDir, -yMove * MOUSE_LOOK_SENSITIVITY, yRotAxis); //rotate vertically
		if (m_lookDir.y > 0.99f || m_lookDir.y < -0.99f) //limit vertical look movement
			m_lookDir = tmp;
	}
	return false;
}