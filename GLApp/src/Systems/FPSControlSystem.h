#pragma once

#include "Core.h"

#include "Input/Input.h"

#include "3rdparty/entityx/System.h"

class FPSControlSystem : public entityx::System<FPSControlSystem>
{
public:
	FPSControlSystem();
	~FPSControlSystem() {};

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:

	Input::MouseMovedListener m_mouseMovedListener;
	Input::KeyDownListener m_keyDownListener;

	float m_speedMultiplier = 1.0f;

	int m_xMoveAmount = 0;
	int m_yMoveAmount = 0;
};