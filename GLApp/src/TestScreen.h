#pragma once

#include "Core.h"
#include "Input/Input.h"
#include "Database/AssetDatabase.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/Utils/FPSCameraController.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Utils/FPSMeasurer.h"


class TestScreen
{
public:
	TestScreen();
	TestScreen(const TestScreen& copy) = delete;
	~TestScreen() {}

	void render(float deltaSec);

private:

	AssetDatabase m_ifcDB;
	AssetDatabase m_objDB;

	GLScene m_ifcScene;
	GLScene m_objScene;
	GLScene m_skyboxScene;

	FPSCameraController m_cameraController;
	PerspectiveCamera m_camera;

	LightManager m_lightManager;
	GLRenderer m_renderer;

	FPSMeasurer m_fpsMeasurer;
	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;
};