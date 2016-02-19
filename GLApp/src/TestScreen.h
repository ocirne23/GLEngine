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
class ListboxItem;
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
	void setSunRotation(float angle);
	void checkboxSelectionChanged(const CEGUI::EventArgs& e);

private:

	enum EGameObjects 
	{ 
		EGameObjects_PALACE, 
		EGameObjects_SKYSPHERE, 
		EGameObjects_TESTSPHERE, 
		EGameObjects_NUM_GAMEOBJECTS 
	};

	const eastl::string m_objectNames[EGameObjects_NUM_GAMEOBJECTS] = {
		"palace.obj",
		"skysphere.obj",
		"sphere.obj"
	};

	AssetDatabase m_objDB;

	GameObject m_gameObjects[EGameObjects_NUM_GAMEOBJECTS];
	GLScene m_scenes[EGameObjects_NUM_GAMEOBJECTS];
	owner<CEGUI::ListboxItem*> m_objectListItems[EGameObjects_NUM_GAMEOBJECTS];
	
	PerspectiveCamera m_camera;
	FPSCameraController m_cameraController;
	CEGUIManager m_guiManager;
	LightManager m_lightManager;
	GLRenderer m_renderer;
	FPSMeasurer m_fpsMeasurer;

	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;

	float m_timeAccum   = 0.0f;
	float m_sunRotation = 0.0f;
};