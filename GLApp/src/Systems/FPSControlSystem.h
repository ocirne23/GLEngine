#pragma once

#include "Core.h"

#include "entityx/System.h"

class FPSControlSystem : public entityx::System<FPSControlSystem>
{
public:
	FPSControlSystem();
	~FPSControlSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	bool mouseMoved(uint xPos, uint yPos, int xMove, int yMove);

private:

	int m_xMoveAmount = 0;
	int m_yMoveAmount = 0;
};