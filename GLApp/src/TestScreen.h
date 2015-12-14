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

	void setSunRotation(float angle);

private:

	AssetDatabase m_ifcDB;
	AssetDatabase m_objDB;

	GLScene m_ifc1Scene;
	GLScene m_ifc2Scene;
	GLScene m_palaceScene;
	GLScene m_skysphereScene;

	FPSCameraController m_cameraController;
	PerspectiveCamera m_camera;

	LightManager m_lightManager;
	GLRenderer m_renderer;

	FPSMeasurer m_fpsMeasurer;
	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;

	float m_timeAccum = 0.0f;
	float m_sunRotation = 0.0f;
};