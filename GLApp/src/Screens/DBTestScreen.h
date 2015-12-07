#pragma once

#include "Core.h"
#include "Input/Input.h"
#include "Database/AssetDatabase.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"
#include "AppUtils/FPSMeasurer.h"

#include "Graphics/Utils/FPSCameraController.h"

class DBTestScreen
{
public:
	DBTestScreen();
	DBTestScreen(const DBTestScreen& copy) = delete;
	~DBTestScreen();

	void render(float deltaSec);

private:

	AssetDatabase m_ifcDB;
	AssetDatabase m_objDB;

	GLScene m_ifcScene;
	GLScene m_objScene;

	GLRenderer m_renderer;
	PerspectiveCamera m_camera;
	FPSCameraController m_cameraController;

	LightManager m_lightManager;

	FPSMeasurer m_fpsMeasurer;

	Input::KeyDownListener m_keyDownListener;
	Input::KeyUpListener m_keyUpListener;
	Input::MouseMovedListener m_mouseMovedListener;
	Input::MouseDownListener m_mouseDownListener;
	Input::MouseUpListener m_mouseUpListener;
	Input::WindowQuitListener m_windowQuitListener;
};