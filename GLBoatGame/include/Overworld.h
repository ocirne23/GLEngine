#pragma once

#include "Core.h"

#include "Graphics/Utils/Box2DDebugDraw.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/FPSCameraController.h"
#include "Utils/FPSMeasurer.h"

class b2World;

class Overworld
{
public:

	Overworld();
	~Overworld();

	void update(float deltaSec);

private:

	float m_timestepAccumulator = 0.0f;
	owner<b2World*> m_physicsWorld = NULL;

	PerspectiveCamera m_camera;
	Box2DDebugDraw m_box2dDebugDraw;
	FPSCameraController m_cameraController;
	FPSMeasurer m_fpsMeasurer;
};