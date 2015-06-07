#pragma once

#include "3rdparty/entityx/entityx.h"
#include "HBAO/HBAOTest.h"

class HBAOTestScreen
{
public:
	HBAOTestScreen();
	~HBAOTestScreen();

	void render(float deltaSec);

private:

	entityx::EntityX m_entityx;
	PerspectiveCamera* m_camera = NULL;
	HBAOTest m_test;
};