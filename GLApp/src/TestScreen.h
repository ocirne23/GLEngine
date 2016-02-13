#pragma once

#include "Input/Input.h"
#include "Database/AssetDatabase.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/Utils/FPSCameraController.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Utils/FPSMeasurer.h"
#include "Graphics/UI/CEGUIManager.h"
#include "GameObject.h"

class DBScene;
namespace CEGUI { class EventArgs; }

class TestScreen
{
public:
	TestScreen();
	TestScreen(const TestScreen& copy) = delete;
	~TestScreen() {}

	void render(float deltaSec);

private:

	void initializeInputListeners();
	void initializeGUI();
	void addWindow(CEGUI::Window* window);
	void setSunRotation(float angle);
	void createTestSphere(DBScene& sphereDB, const eastl::string& materialName, const glm::vec3& position);
	void checkboxSelectionChanged(const CEGUI::EventArgs& e);

private:

	AssetDatabase m_ifcDB;
	AssetDatabase m_objDB;

	enum EGameObjects { EGameObjects_PALACE, EGameObjects_SKYSPHERE, EGameObjects_TESTSPHERE, EGameObjects_IFC1, EGameObjects_IFC2, EGameObjects_NUM_GAMEOBJECTS };
	GameObject m_gameObjects[EGameObjects_NUM_GAMEOBJECTS];
	GLScene m_scenes[EGameObjects_NUM_GAMEOBJECTS];

	FPSCameraController m_cameraController;
	PerspectiveCamera m_camera;

	LightManager m_lightManager;
	GLRenderer m_renderer;
	
	CEGUIManager m_guiManager;

	FPSMeasurer m_fpsMeasurer;
	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;

	float m_timeAccum = 0.0f;
	float m_sunRotation = 0.0f;
};