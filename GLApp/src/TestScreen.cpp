#include "TestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Utils/StringUtils.h"
#include "Database/Assets/DBScene.h"

#include <CEGUI/EventArgs.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/All.h>

#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>

TestScreen::TestScreen() : m_lightManager(GLConfig::getMaxLights())
{
	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_camera.initialize(float(viewportWidth), float(viewportHeight), 90.0f, 0.1f, 1000.0f);
	m_camera.lookAtPoint(glm::vec3(0.0f, 0.0f, 20.0f));
	m_renderer.initialize(m_camera);
	m_cameraController.setCameraSpeed(5.0f);

	if (m_objDB.openExisting("assets/OBJ-DB.da"))
	{
		if (m_objDB.hasAsset(m_objectNames[EGameObjects_PALACE]))
		{
			m_scenes[EGameObjects_PALACE].initialize(m_objectNames[EGameObjects_PALACE], m_objDB);
			m_gameObjects[EGameObjects_PALACE].initialize(&m_scenes[EGameObjects_PALACE]);
			m_gameObjects[EGameObjects_PALACE].setPosition(glm::vec3(0.0f, -1.5f, 20.0f));
			m_gameObjects[EGameObjects_PALACE].setScale(0.5f);
			m_renderer.addRenderObject(&m_gameObjects[EGameObjects_PALACE]);
		}
		if (m_objDB.hasAsset(m_objectNames[EGameObjects_SKYSPHERE]))
		{
			m_scenes[EGameObjects_SKYSPHERE].initialize(m_objectNames[EGameObjects_SKYSPHERE], m_objDB);
			m_gameObjects[EGameObjects_SKYSPHERE].initialize(&m_scenes[EGameObjects_SKYSPHERE]);
			m_renderer.addSkybox(&m_gameObjects[EGameObjects_SKYSPHERE]);
		}
	}

	m_fpsMeasurer.setLogFunction(1.0f, [this]() { 
		GLEngine::graphics->setWindowTitle(("GLApp FPS: " + StringUtils::to_string(m_fpsMeasurer.getAvgFps())).c_str()); 
	});
	setSunRotation(m_sunRotation);

	m_guiManager.initialize();

	initializeGUI();
	initializeInputListeners();
}

TestScreen::~TestScreen()
{
	for (owner<CEGUI::ListboxItem*> item : m_objectListItems)
		delete item;
}

void TestScreen::render(float a_deltaSec)
{
	m_timeAccum += a_deltaSec;
	if (m_timeAccum > 1.0f)
	{
		m_timeAccum = 0.0f;
	//	setSunRotation(m_sunRotation);
		m_sunRotation += 1.0f;
	}

	m_fpsMeasurer.tickFrame(a_deltaSec);
	m_cameraController.update(m_camera, a_deltaSec, !m_guiManager.isFocused());
	m_renderer.render(m_camera, m_lightManager);
	m_guiManager.render(a_deltaSec);

	GLEngine::graphics->swap();
}

void TestScreen::setSunRotation(float a_angle)
{	// Values based on skysphere
	const glm::vec3 baseSunDir = glm::normalize(glm::vec3(0.597, 0.537, 0.597));
	const glm::vec3 sunColor = glm::vec3(0.75f, 0.7f, 0.66f);
	const glm::mat4 sunRot = glm::rotate(a_angle, glm::vec3(0, 1, 0));
	const glm::vec3 sunPos = glm::mat3(sunRot) * baseSunDir;
	const float skyboxAngleOffset = 140.0f;

	m_renderer.setSun(sunPos, sunColor, 1.0f);
	m_gameObjects[EGameObjects_SKYSPHERE].setRotation(glm::vec3(0, 1, 0), a_angle - skyboxAngleOffset);
}

void TestScreen::initializeGUI()
{
	{
		CEGUI::FrameWindow* objectListFrameWindow = scast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().loadLayoutFromFile("GLEngine/ObjectListFrameWindow.layout"));
		CEGUI::Listbox* objectListBox = scast<CEGUI::Listbox*>(objectListFrameWindow->getChild("ObjectListBox"));
		
		for (uint i = 0; i < EGameObjects_NUM_GAMEOBJECTS; ++i)
		{
			m_objectListItems[i] = new CEGUI::ListboxTextItem(m_objectNames[i].c_str());
			objectListBox->addItem(m_objectListItems[i]);
		}
		objectListFrameWindow->setDragMovingEnabled(false);
		objectListFrameWindow->setRolledup(true);
		addWindow(objectListFrameWindow);
	}
	{
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
		// AA buttons
		{
			CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("NoAARadioButton"));
			checkbox->setGroupID(1);
			checkbox->setSelected(!m_renderer.isFXAAEnabled() && GLConfig::getMultisampleType() == GLFramebuffer::EMultiSampleType::NONE);
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
			checkbox->setSelected(GLConfig::getMultisampleType() == GLFramebuffer::EMultiSampleType::MSAA_2X);
			checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
		}
		{
			CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("MSAA4RadioButton"));
			checkbox->setGroupID(1);
			checkbox->setSelected(GLConfig::getMultisampleType() == GLFramebuffer::EMultiSampleType::MSAA_4X);
			checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
		}
		{
			CEGUI::RadioButton* checkbox = scast<CEGUI::RadioButton*>(optionsFrameWindow->getChild("MSAA8RadioButton"));
			checkbox->setGroupID(1);
			checkbox->setSelected(GLConfig::getMultisampleType() == GLFramebuffer::EMultiSampleType::MSAA_8X);
			checkbox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&TestScreen::checkboxSelectionChanged, this));
		}

		optionsFrameWindow->setRolledup(true);
		optionsFrameWindow->setDragMovingEnabled(false);
		addWindow(optionsFrameWindow);
	}
}

void TestScreen::addWindow(CEGUI::Window* a_window)
{
	CEGUI::Window* root = m_guiManager.getRootWindow();
	if (root->isChild(a_window->getName()))
		root->removeChild(a_window->getName());
	root->addChild(a_window);
}

BEGIN_UNNAMED_NAMESPACE()

const glm::vec3 DIRECTIONS[] = {
	glm::vec3(1, 0, 0),
	glm::vec3(-1, 0, 0),
	glm::vec3(0.0000001, 1, 0),
	glm::vec3(0.0000001, -1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1)
};

END_UNNAMED_NAMESPACE()

int da = 0;

void TestScreen::initializeInputListeners()
{
	m_windowQuitListener.setFunc([]() { GLEngine::shutdown(); });
	m_keyDownListener.setFunc([this](EKey a_key, bool a_isRepeat)
	{
		if (a_isRepeat)
			return;

		switch (a_key)
		{
		case EKey::KP_1:
		{
			m_camera.lookAtDir(DIRECTIONS[da++]);
			if (da > 5)
				da = 0;
			break;
		}
		case EKey::ESCAPE:   GLEngine::shutdown(); break;
		case EKey::KP_5:     initializeGUI(); break;
		case EKey::KP_6:     m_renderer.reloadShaders(); break;
		case EKey::KP_7:     m_renderer.setDepthPrepassEnabled(!m_renderer.isDepthPrepassEnabled()); break;
		case EKey::KP_PLUS:  m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 1.2f); break;
		case EKey::KP_MINUS: m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 0.8f); break;
		case EKey::Y:        m_lightManager.deleteLights(); break;
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
		GLConfig::setMultisampleType(isSelected ? GLFramebuffer::EMultiSampleType::MSAA_2X : GLFramebuffer::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	} 
	else if (name == "MSAA4RadioButton") 
	{
		GLConfig::setMultisampleType(isSelected ? GLFramebuffer::EMultiSampleType::MSAA_4X : GLFramebuffer::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	}
	else if (name == "MSAA8RadioButton") 
	{
		GLConfig::setMultisampleType(isSelected ? GLFramebuffer::EMultiSampleType::MSAA_8X : GLFramebuffer::EMultiSampleType::NONE);
		m_renderer.initialize(m_camera);
	}
}
