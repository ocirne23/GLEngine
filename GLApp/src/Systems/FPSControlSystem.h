#pragma once

#include "Core.h"

#include "entityx/System.h"
#include "Input/MouseListener.h"

class FPSControlSystem : public entityx::System<FPSControlSystem>, public MouseListener
{
public:
	FPSControlSystem();
	~FPSControlSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	virtual bool mouseMoved(int xPos, int yPos, int xMove, int yMove) OVERRIDE;

private:

	int m_xMoveAmount = 0;
	int m_yMoveAmount = 0;
};