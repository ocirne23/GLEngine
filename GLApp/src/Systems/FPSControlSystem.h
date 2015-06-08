#pragma once

#include "Core.h"

#include "Input/Input.h"

#include "3rdparty/entityx/System.h"

class FPSControlSystem : public entityx::System<FPSControlSystem>
{
public:
	FPSControlSystem();
	~FPSControlSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);

private:

	Input::MouseMovedListener m_mouseMovedListener;

	int m_xMoveAmount = 0;
	int m_yMoveAmount = 0;
};