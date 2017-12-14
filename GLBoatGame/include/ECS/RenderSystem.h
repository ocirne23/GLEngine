#pragma once

#include "Core.h"

#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/FPSCameraController.h"
#include "Utils/FPSMeasurer.h"

class BoatGame;

class RenderSystem
{
public:

	RenderSystem(BoatGame& boatGame);
	~RenderSystem();

	void render(float a_deltaSec);

	PerspectiveCamera& getCamera() { return m_camera; }

private:

	BoatGame& m_boatGame;

	PerspectiveCamera m_camera;
	FPSCameraController m_cameraController;
	FPSMeasurer m_fpsMeasurer;
};