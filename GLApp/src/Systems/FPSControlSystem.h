#pragma once

#include "Core.h"
#include "Input/Input.h"
#include "3rdparty/entityx/System.h"
#include <glm/glm.hpp>

class FPSControlSystem : public entityx::System<FPSControlSystem>
{
public:
	FPSControlSystem();
	~FPSControlSystem() {};

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:

	glm::vec3 getLocalSpaceMovementVector();

private:

	Input::MouseMovedListener m_mouseMovedListener;
	Input::KeyDownListener m_keyDownListener;

	int m_mouseXMoveAmount = 0;
	int m_mouseYMoveAmount = 0;
};