#pragma once

#include "Input/Input.h"

#define CEGUI_STATIC
#define PCRE_STATIC
#define CEGUIBASE_EXPORTS

BEGIN_NAMESPACE(CEGUI)
class Window;
class GUIContext;
END_NAMESPACE(CEGUI)

class CEGUIManager
{
public:

	void initialize();
	void render(float deltaSec);

	bool isFocused() const         { return m_isFocused; }
	CEGUI::Window* getRootWindow() { return m_rootWindow; }
	bool isInitialized() const     { return m_initialized; }

private:

	void initializeInput();

private:

	bool m_initialized              = false;
	bool m_isFocused                = false;
	CEGUI::Window* m_rootWindow     = NULL;
	CEGUI::GUIContext* m_guiContext = NULL;

	Input::KeyDownListener m_keyDownListener;
	Input::KeyUpListener m_keyUpListener;
	Input::TextInputListener m_textInputListener;
	Input::MouseDownListener m_mouseDownListener;
	Input::MouseUpListener m_mouseUpListener;
	Input::MouseMovedListener m_mouseMovedListener;
	Input::MouseScrolledListener m_mouseScrolledListener;
};