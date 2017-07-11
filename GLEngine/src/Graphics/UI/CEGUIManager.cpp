#include "Graphics/UI/CEGUIManager.h"

#include "Graphics/GL/GL.h"
#include "Graphics/Utils/ARBDebugOutput.h"

// #define NOMINMAX

#include <CEGUI/DefaultResourceProvider.h>
#include <CEGUI/FontManager.h>
#include <CEGUI/GUIContext.h>
#include <CEGUI/ImageManager.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/ScriptModule.h>
#include <CEGUI/System.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/XMLParser.h>
#include <CEGUI/falagard/WidgetLookManager.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

#define USE_GUI_CURSOR 0

BEGIN_UNNAMED_NAMESPACE()

CEGUI::MouseButton getCEMouseButton(EMouseButton a_button)
{
	switch (a_button)
	{
	case EMouseButton::LEFT:   return CEGUI::MouseButton::LeftButton;
	case EMouseButton::MIDDLE: return CEGUI::MouseButton::MiddleButton;
	case EMouseButton::RIGHT:  return CEGUI::MouseButton::RightButton;
	case EMouseButton::X1:     return CEGUI::MouseButton::X1Button;
	case EMouseButton::X2:     return CEGUI::MouseButton::X2Button;
	default:                   return CEGUI::MouseButton::NoButton;
	}
}

CEGUI::Key::Scan getCEScanCode(EKey a_key)
{
	switch (a_key)
	{
	case EKey::A: return CEGUI::Key::A;
	case EKey::B: return CEGUI::Key::B;
	case EKey::C: return CEGUI::Key::C;
	case EKey::D: return CEGUI::Key::D;
	case EKey::E: return CEGUI::Key::E;
	case EKey::F: return CEGUI::Key::F;
	case EKey::G: return CEGUI::Key::G;
	case EKey::H: return CEGUI::Key::H;
	case EKey::I: return CEGUI::Key::I;
	case EKey::J: return CEGUI::Key::J;
	case EKey::K: return CEGUI::Key::K;
	case EKey::L: return CEGUI::Key::L;
	case EKey::M: return CEGUI::Key::M;
	case EKey::N: return CEGUI::Key::N;
	case EKey::O: return CEGUI::Key::O;
	case EKey::P: return CEGUI::Key::P;
	case EKey::Q: return CEGUI::Key::Q;
	case EKey::R: return CEGUI::Key::R;
	case EKey::S: return CEGUI::Key::S;
	case EKey::T: return CEGUI::Key::T;
	case EKey::U: return CEGUI::Key::U;
	case EKey::V: return CEGUI::Key::V;
	case EKey::W: return CEGUI::Key::W;
	case EKey::X: return CEGUI::Key::X;
	case EKey::Y: return CEGUI::Key::Y;
	case EKey::Z: return CEGUI::Key::Z;

	case EKey::N1: return CEGUI::Key::One;
	case EKey::N2: return CEGUI::Key::Two;
	case EKey::N3: return CEGUI::Key::Three;
	case EKey::N4: return CEGUI::Key::Four;
	case EKey::N5: return CEGUI::Key::Five;
	case EKey::N6: return CEGUI::Key::Six;
	case EKey::N7: return CEGUI::Key::Seven;
	case EKey::N8: return CEGUI::Key::Eight;
	case EKey::N9: return CEGUI::Key::Nine;
	case EKey::N0: return CEGUI::Key::Zero;

	case EKey::RETURN:    return CEGUI::Key::Return;
	case EKey::ESCAPE:    return CEGUI::Key::Escape;
	case EKey::BACKSPACE: return CEGUI::Key::Backspace;
	case EKey::TAB:       return CEGUI::Key::Tab;
	case EKey::SPACE:     return CEGUI::Key::Space;

	case EKey::Minus:        return CEGUI::Key::Minus;
	case EKey::EQUALS:       return CEGUI::Key::Equals;
	case EKey::LEFTBRACKET:  return CEGUI::Key::LeftBracket;
	case EKey::RIGHTBRACKET: return CEGUI::Key::RightBracket;
	case EKey::BACKSLASH:    return CEGUI::Key::Backslash;
	case EKey::NONUSHASH:    return CEGUI::Key::Unknown; // what?

	case EKey::SEMICOLON:  return CEGUI::Key::Semicolon;
	case EKey::APOSTROPHE: return CEGUI::Key::Apostrophe;
	case EKey::GRAVE:      return CEGUI::Key::Grave;
	case EKey::COMMA:      return CEGUI::Key::Comma;
	case EKey::PERIOD:     return CEGUI::Key::Period;
	case EKey::SLASH:      return CEGUI::Key::Slash;
	case EKey::CAPSLOCK:   return CEGUI::Key::Capital;

	case EKey::F1:  return CEGUI::Key::F1;
	case EKey::F2:  return CEGUI::Key::F2;
	case EKey::F3:  return CEGUI::Key::F3;
	case EKey::F4:  return CEGUI::Key::F4;
	case EKey::F5:  return CEGUI::Key::F5;
	case EKey::F6:  return CEGUI::Key::F6;
	case EKey::F7:  return CEGUI::Key::F7;
	case EKey::F8:  return CEGUI::Key::F8;
	case EKey::F9:  return CEGUI::Key::F9;
	case EKey::F10: return CEGUI::Key::F10;
	case EKey::F11: return CEGUI::Key::F11;
	case EKey::F12: return CEGUI::Key::F12;

	case EKey::PRINTSCREEN: return CEGUI::Key::Unknown; // ?
	case EKey::SCROLLLOCK:  return CEGUI::Key::ScrollLock;
	case EKey::PAUSE:       return CEGUI::Key::Pause;
	case EKey::INSERT:      return CEGUI::Key::Insert;
	case EKey::HOME:        return CEGUI::Key::Home;
	case EKey::PAGEUP:      return CEGUI::Key::PageUp;
	case EKey(76):          return CEGUI::Key::Delete; //EKey::DELETE (DELETE is defined somewhere xD)
	case EKey::END:         return CEGUI::Key::End;
	case EKey::PAGEDOWN:    return CEGUI::Key::PageDown;
	case EKey::RIGHT:       return CEGUI::Key::ArrowRight;
	case EKey::LEFT:        return CEGUI::Key::ArrowLeft;
	case EKey::DOWN:        return CEGUI::Key::ArrowDown;
	case EKey::UP:          return CEGUI::Key::ArrowUp;

	case EKey::NUMLOCKCLEAR: return CEGUI::Key::NumLock;
	case EKey::KP_DIVIDE:    return CEGUI::Key::Divide;
	case EKey::KP_MULTIPLY:  return CEGUI::Key::Multiply;
	case EKey::KP_MINUS:     return CEGUI::Key::Subtract;
	case EKey::KP_PLUS:      return CEGUI::Key::Add;
	case EKey::KP_ENTER:     return CEGUI::Key::NumpadEnter;
	case EKey::KP_1:         return CEGUI::Key::Numpad1;
	case EKey::KP_2:         return CEGUI::Key::Numpad2;
	case EKey::KP_3:         return CEGUI::Key::Numpad3;
	case EKey::KP_4:         return CEGUI::Key::Numpad4;
	case EKey::KP_5:         return CEGUI::Key::Numpad5;
	case EKey::KP_6:         return CEGUI::Key::Numpad6;
	case EKey::KP_7:         return CEGUI::Key::Numpad7;
	case EKey::KP_8:         return CEGUI::Key::Numpad8;
	case EKey::KP_9:         return CEGUI::Key::Numpad9;
	case EKey::KP_0:         return CEGUI::Key::Numpad0;
	case EKey::KP_PERIOD:    return CEGUI::Key::Decimal;
	case EKey::KP_EQUALS:    return CEGUI::Key::NumpadEquals;

	case EKey::F13: return CEGUI::Key::F13;
	case EKey::F14: return CEGUI::Key::F14;
	case EKey::F15: return CEGUI::Key::F15;
	case EKey::F16: return CEGUI::Key::Unknown;
	case EKey::F17: return CEGUI::Key::Unknown;
	case EKey::F18: return CEGUI::Key::Unknown;
	case EKey::F19: return CEGUI::Key::Unknown;
	case EKey::F20: return CEGUI::Key::Unknown;
	case EKey::F21: return CEGUI::Key::Unknown;
	case EKey::F22: return CEGUI::Key::Unknown;
	case EKey::F23: return CEGUI::Key::Unknown;
	case EKey::F24: return CEGUI::Key::Unknown;

	case EKey::LCTRL:  return CEGUI::Key::LeftControl;
	case EKey::LSHIFT: return CEGUI::Key::LeftShift;
	case EKey::LALT:   return CEGUI::Key::LeftAlt;
	case EKey::LGUI:   return CEGUI::Key::LeftWindows;
	case EKey::RCTRL:  return CEGUI::Key::RightControl;
	case EKey::RSHIFT: return CEGUI::Key::RightShift;
	case EKey::RALT:   return CEGUI::Key::RightAlt;
	case EKey::RGUI:   return CEGUI::Key::RightWindows;

	default: return CEGUI::Key::Unknown;
	}
}

END_UNNAMED_NAMESPACE()

void CEGUIManager::initialize()
{
	if (m_initialized)
		CEGUI::OpenGL3Renderer::destroySystem();

	ARBDebugOutput::setDebugLog(false);
	CEGUI::OpenGL3Renderer::bootstrapSystem();
	ARBDebugOutput::setDebugLog(true);

	CEGUI::System& system = CEGUI::System::getSingleton();
	CEGUI::WindowManager& windowManager = CEGUI::WindowManager::getSingleton();
	CEGUI::SchemeManager& schemeManager = CEGUI::SchemeManager::getSingleton();
	CEGUI::FontManager& fontManager = CEGUI::FontManager::getSingleton();

	// initialise the required dirs for the DefaultResourceProvider
	CEGUI::DefaultResourceProvider* rp = scast<CEGUI::DefaultResourceProvider*>(system.getResourceProvider());
	rp->setResourceGroupDirectory("schemes", "assets/CEGUI/schemes/");
	rp->setResourceGroupDirectory("imagesets", "assets/CEGUI/imagesets/");
	rp->setResourceGroupDirectory("fonts", "assets/CEGUI/fonts/");
	rp->setResourceGroupDirectory("layouts", "assets/CEGUI/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "assets/CEGUI/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "assets/CEGUI/lua_scripts/");
	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	// setup default group for validation schemas
	CEGUI::XMLParser* parser = system.getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");

	//schemeManager.createFromFile("AlfiskoSkin.scheme");
	//schemeManager.createFromFile("GameMenu.scheme");
	schemeManager.createFromFile("Generic.scheme");
	//schemeManager.createFromFile("HUDDemo.scheme");
	//schemeManager.createFromFile("OgreTray.scheme");
	schemeManager.createFromFile("SampleBrowser.scheme");
	schemeManager.createFromFile("TaharezLook.scheme");
	//schemeManager.createFromFile("VanillaCommonDialogs.scheme");
	schemeManager.createFromFile("VanillaSkin.scheme");
	schemeManager.createFromFile("WindowsLook.scheme");

	fontManager.createFromFile("DejaVuSans-10.font");

	m_guiContext = &system.getDefaultGUIContext();
	m_guiContext->setDefaultFont("DejaVuSans-10");
	m_guiContext->setDefaultTooltipType("TaharezLook/Tooltip");
#if USE_GUI_CURSOR
	m_guiContext->getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
#endif

	m_rootWindow = windowManager.createWindow("DefaultWindow", "root");
	m_guiContext->setRootWindow(m_rootWindow);

	initializeInput();

	m_initialized = true;
}

void CEGUIManager::render(float a_deltaSec)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	m_isFocused = (m_guiContext->getWindowContainingMouse() != m_rootWindow);
	CEGUI::System::getSingleton().injectTimePulse(a_deltaSec);
	CEGUI::System::getSingleton().renderAllGUIContexts();
}

void CEGUIManager::initializeInput()
{
	m_mouseMovedListener.setFunc([this](uint a_xPos, uint a_yPos, int a_deltaX, int a_deltaY) {
		m_guiContext->injectMousePosition(float(a_xPos), float(a_yPos));
	});
	m_keyDownListener.setFunc([this](EKey a_key, bool a_isRepeat) {
		if (isFocused()) m_guiContext->injectKeyDown(getCEScanCode(a_key));
	});
	m_keyUpListener.setFunc([this](EKey a_key) {
		if (isFocused()) m_guiContext->injectKeyUp(getCEScanCode(a_key));
	});
	m_textInputListener.setFunc([this](const char* a_text) {
		if (isFocused()) m_guiContext->injectChar(*rcast<const CEGUI::String::value_type*>(a_text));
	});
	m_mouseDownListener.setFunc([this](EMouseButton a_button, uint a_xPos, uint a_yPos) {
		if (isFocused()) m_guiContext->injectMouseButtonDown(getCEMouseButton(a_button));
	});
	m_mouseUpListener.setFunc([this](EMouseButton a_button, uint a_xPos, uint a_yPos) {
		if (isFocused()) m_guiContext->injectMouseButtonUp(getCEMouseButton(a_button));
	});
	m_mouseScrolledListener.setFunc([this](uint a_amount) {
		if (isFocused()) m_guiContext->injectMouseWheelChange(float(a_amount));
	});
}

