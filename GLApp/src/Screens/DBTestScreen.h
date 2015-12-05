#pragma once

#include "Core.h"
#include "Input/Input.h"
#include "Database/AssetDatabase.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/Utils/PerspectiveCamera.h"

#include "Graphics/Utils/FPSCameraController.h"

class DBTestScreen
{
public:
	DBTestScreen();
	DBTestScreen(const DBTestScreen& copy) = delete;
	~DBTestScreen();

	void render(float deltaSec);

private:

	void keyDown(EKey key);

private:

	Input::KeyDownListener m_keyDownListener;
	Input::KeyUpListener m_keyUpListener;

	Input::MouseMovedListener m_mouseMovedListener;
	Input::MouseDownListener m_mouseDownListener;

	Input::WindowQuitListener m_windowQuitListener;


	AssetDatabase m_db;
	GLScene m_scene;
	GLRenderer m_renderer;
	GLShader m_basicSceneShader;
	PerspectiveCamera m_camera;

	FPSCameraController m_cameraController;
};