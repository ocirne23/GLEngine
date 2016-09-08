#pragma once

#include "Database/AssetDatabase.h"
#include "EASTL/string.h"
#include "GameObject.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/Utils/FPSCameraController.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/UI/CEGUIManager.h"
#include "Input/Input.h"
#include "Utils/FPSMeasurer.h"

class DBScene;
BEGIN_NAMESPACE(CEGUI)
class EventArgs; 
END_NAMESPACE(CEGUI)

class TestScreen
{
public:
	TestScreen();
	TestScreen(const TestScreen& copy) = delete;
	~TestScreen();

	void render(float deltaSec);

private:

	void initializeInputListeners();
	void initializeGUI();
	void addWindow(CEGUI::Window* window);
	void setSunDirection(glm::vec3 direction);
	void checkboxSelectionChanged(const CEGUI::EventArgs& e);

private:

	AssetDatabase m_objDB;

	GameObject m_skysphere, m_sponza, m_sun;
	GLScene m_skysphereScene, m_sponzaScene, m_sunScene;
	
	PerspectiveCamera m_camera;
	FPSCameraController m_cameraController;
	CEGUIManager m_guiManager;
	LightManager m_lightManager;
	GLRenderer m_renderer;
	FPSMeasurer m_fpsMeasurer;

	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;

	float m_timeAccum = 0.0f;
	glm::vec3 m_sunDir;
};