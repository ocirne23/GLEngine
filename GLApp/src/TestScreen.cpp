#include "TestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Utils/StringUtils.h"

#include "Network/TCPSocket.h"

#include <CEGUI/EventArgs.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/All.h>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

TestScreen::TestScreen() : m_lightManager(GLConfig::getMaxLights())
{
	/*
	GLEngine::createThread("TCPListenThread", []()
	{
		byte buffer[512];

		TCPSocket s;
		s.listen("localhost", 23232);
		while (s.receive(buffer, [](gsl::span<byte> data)
		{
			print("received: %i - %s\n", data.length_bytes(), rcast<const char*>(data.data()));
		}));
	});
	
	GLEngine::createThread("TCPSendThread", []()
	{
		byte data[] = "ehuehue";

		TCPSocket s;
		s.connect("localhost", 23232);
		for (uint i = 0; i < 10; ++i)
		{
			s.send(data);
		}
	});
	*/
	GLEngine::graphics->setVsync(false);
	const uint viewportWidth = GLEngine::graphics->getViewportWidth();
	const uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_camera.initialize(float(viewportWidth), float(viewportHeight), 90.0f, 0.1f, 1000.0f);
	m_camera.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

	m_renderer.initialize(m_camera);
	m_cameraController.setCameraSpeed(5.0f);

	if (m_objDB.openExisting("assets/OBJ-DB.da"))
	{
		m_sponzaScene.initialize("sponza.obj", m_objDB);
		m_skysphereScene.initialize("skysphere.obj", m_objDB);
		m_skysphereScene.setAsSkybox(true);
		m_sunScene.initialize("sphere.obj", m_objDB);
		m_sunScene.setAsSkybox(true);

		m_sponza.initialize(&m_sponzaScene);
		m_renderer.addRenderObject(&m_sponza);

		m_skysphere.initialize(&m_skysphereScene);
		m_renderer.addSkybox(&m_skysphere);

		m_sun.initialize(&m_sunScene);
		m_sun.setScale(30.0f);
		m_renderer.addRenderObject(&m_sun);
	}

	setSunDirection(glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

	m_fpsMeasurer.setLogFunction(1.0f, [this]() { 
		GLEngine::graphics->setWindowTitle(("GLApp FPS: " + StringUtils::to_string(m_fpsMeasurer.getAvgFps())).c_str()); 
	});

	initializeGUI();
	initializeInputListeners();
}

TestScreen::~TestScreen()
{
}

void TestScreen::render(float a_deltaSec)
{
	// Quick & dirty rotating sun.
	m_timeAccum += a_deltaSec;
	if (m_timeAccum > 1.0f)
	{
		m_timeAccum = 0.0f; 
		setSunDirection(glm::rotate(m_sunDir, glm::abs(m_sunDir.y) > 0.60f ? 0.02f : -1.8f, glm::normalize(glm::vec3(0.5f, 0.0f, 0.5f))));
	}
	m_sun.setPosition(m_camera.getPosition() + m_sunDir * 900.0f);

	m_fpsMeasurer.tickFrame(a_deltaSec);
	m_cameraController.update(m_camera, a_deltaSec, !m_guiManager.isFocused());
	m_renderer.render(m_camera, m_lightManager);
	m_guiManager.render(a_deltaSec);

	GLEngine::graphics->swap();
}

void TestScreen::setSunDirection(glm::vec3 a_direction)
{
	m_sunDir = a_direction;
	m_renderer.setSun(m_sunDir, glm::vec3(0.75f, 0.7f, 0.66f), 1.0f);
}

void TestScreen::initializeGUI()
{
	m_guiManager.initialize();

	CEGUI::FrameWindow* optionsFrameWindow = scast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().loadLayoutFromFile("GLEngine/OptionsFrameWindow.layout"));
	{
		CEGUI::ToggleButton* checkbox = scast<CEGUI::ToggleButton*>(optionsFrameWindow->getChild("HBAOCheckBox"));
		checkbox->setSelected(m_renderer.isHBAOEnabled());
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::ToggleButton* checkbox = scast<CEGUI::ToggleButton*>(optionsFrameWindow->getChild("BloomCheckBox"));
		checkbox->setSelected(m_renderer.isBloomEnabled());
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::ToggleButton* checkbox = scast<CEGUI::ToggleButton*>(optionsFrameWindow->getChild("ShadowsCheckBox"));
		checkbox->setSelected(m_renderer.isShadowsEnabled());
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{// AA buttons
		CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("NoAARadioButton"));
		checkbox->setGroupID(1);
		checkbox->setSelected(!m_renderer.isFXAAEnabled() && GLConfig::getMultisampleType() == GLTexture::EMultiSampleType::NONE);
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("FXAARadioButton"));
		checkbox->setGroupID(1);
		checkbox->setSelected(m_renderer.isFXAAEnabled());
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("MSAA2RadioButton"));
		checkbox->setGroupID(1);
		checkbox->setSelected(GLConfig::getMultisampleType() == GLTexture::EMultiSampleType::MSAA_2X);
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("MSAA4RadioButton"));
		checkbox->setGroupID(1);
		checkbox->setSelected(GLConfig::getMultisampleType() == GLTexture::EMultiSampleType::MSAA_4X);
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}
	{
		CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("MSAA8RadioButton"));
		checkbox->setGroupID(1);
		checkbox->setSelected(GLConfig::getMultisampleType() == GLTexture::EMultiSampleType::MSAA_8X);
		checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
	}

	optionsFrameWindow->setDragMovingEnabled(false);
	addWindow(optionsFrameWindow);

	CEGUI::FrameWindow* controlsDisplayFrameWindow = scast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().loadLayoutFromFile("GLEngine/ObjectListFrameWindow.layout"));
	controlsDisplayFrameWindow->setDragMovingEnabled(false);
	const CEGUI::String controlsText =
		"WASD / Shift / Space: Movement\n\
		Left Mouse Button : Move camera\n\
		Escape: Shutdown\n\
		T: Place point light\n\
		Y: Delete all point lights\n\
		U: Set sun direction\n\
		Keypad 5: Reload GUI\n\
		Keypad 6: Reload Shaders\n\
		Keypad Plus: Increase camera speed\n\
		Keypad Minus: Decrease camera speed\n\
		Collapse this window by doubleclicking the bar";
	controlsDisplayFrameWindow->getChild("StaticText")->setText(controlsText);
	addWindow(controlsDisplayFrameWindow);
}

void TestScreen::addWindow(CEGUI::Window* a_window)
{
	CEGUI::Window* root = m_guiManager.getRootWindow();
	if (root->isChild(a_window->getName()))
		root->removeChild(a_window->getName());
	root->addChild(a_window);
}

void TestScreen::initializeInputListeners()
{
	m_windowQuitListener.setFunc([]() { GLEngine::shutdown(); });
	m_keyDownListener.setFunc([this](EKey a_key, bool a_isRepeat)
	{
		if (a_isRepeat)
			return;

		switch (a_key)
		{
		case EKey::ESCAPE:   GLEngine::shutdown(); break;
		case EKey::KP_5:     initializeGUI(); break;
		case EKey::KP_6:     m_renderer.reloadShaders(); break;
		case EKey::KP_PLUS:  m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 1.2f); break;
		case EKey::KP_MINUS: m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 0.8f); break;
		case EKey::Y:        m_lightManager.deleteLights(); break;
		case EKey::U:        setSunDirection(-m_camera.getDirection()); break;
		case EKey::T:
		{
			glm::vec3 position = m_camera.getPosition();
			glm::vec3 color = glm::normalize(glm::linearRand(glm::vec3(0.5), glm::vec3(1)));
			float intensity = glm::linearRand(0.8f, 2.0f);
			float radius = glm::linearRand(2.0f, 4.0f);
			m_lightManager.createLight(position, radius, color, intensity);
			break;
		}
		default: break;
		}
	});
}

void TestScreen::checkboxSelectionChanged(const CEGUI::EventArgs& e)
{
	const CEGUI::ToggleButton* checkbox = scast<CEGUI::ToggleButton*>(scast<const CEGUI::WindowEventArgs&>(e).window);
	const bool isSelected = checkbox->isSelected();
	const CEGUI::String name = checkbox->getName();
	if (name == "HBAOCheckBox")
		m_renderer.setHBAOEnabled(isSelected);
	else if (name == "BloomCheckBox")
		m_renderer.setBloomEnabled(isSelected);
	else if (name == "ShadowsCheckBox")
		m_renderer.setShadowsEnabled(isSelected);
	else if (name == "FXAARadioButton")
		m_renderer.setFXAAEnabled(isSelected);
	else if (name == "MSAA2RadioButton") 
	{
		GLConfig::setMultisampleType(isSelected ? GLTexture::EMultiSampleType::MSAA_2X : GLTexture::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	} 
	else if (name == "MSAA4RadioButton") 
	{
		GLConfig::setMultisampleType(isSelected ? GLTexture::EMultiSampleType::MSAA_4X : GLTexture::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	}
	else if (name == "MSAA8RadioButton") 
	{
		GLConfig::setMultisampleType(isSelected ? GLTexture::EMultiSampleType::MSAA_8X : GLTexture::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	}
}
