#pragma once

#include "Core.h"

#include "Input/EKey.h"
#include "Input/EMouseButton.h"
#include "Input/InputListener.h"
#include "Utils/ConcurrentQueue.h"
#include "EASTL/vector.h"

/*** Usage examples ***
Polling input:

if (GLEngine::input->isKeyPressed(EKey::ESCAPE)) {}

Listening to input events:

Input::KeyDownListener keyDownListener;
keyDownListener.setFunc([this](EKey a_key, bool a_isRepeat) { onKeyDown(a_key); });
**********************/
class Input
{
public:
	friend class GLEngine;

	~Input() {}

	bool isKeyPressed(EKey key);
	bool isMousePressed(EMouseButton button);
	void getMousePosition(int& xPos, int& yPos);
	void setMouseCaptured(bool captured);
	bool isMouseCaptured();

private:

	Input() {}

	void pollEvents();
	void processEvents();

	void keyDown(EKey key, bool isRepeat);
	void keyUp(EKey key);
	void textInput(const char* text);
	void mouseDown(EMouseButton button, uint xPos, uint yPos);
	void mouseUp(EMouseButton button, uint xPos, uint yPos);
	void mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY);
	void mouseScrolled(int amount);
	void windowResized(uint width, uint height);
	void windowQuit();

private:

	struct Event { byte data[56]; }; // Matches SDL_Event, to avoid exposing the SDL headers.
	ConcurrentQueue<Event> m_eventQueue;

	// Listener definitions from here on
public: 
	// Unique tag to avoid identical typedefs
	struct KeyDownTag {};
	struct KeyUpTag {};
	struct TextInputTag {};
	struct MouseDownTag {};
	struct MouseUpTag {};
	struct MouseMovedTag {};
	struct MouseScrolledTag {};
	struct WindowResizedTag {};
	struct WindowQuitTag {};
	
	//                    Tag               Ret   Args                      Name
	typedef InputListener<KeyDownTag,       void, EKey, bool>               KeyDownListener;
	typedef InputListener<KeyUpTag,         void, EKey>                     KeyUpListener;
	typedef InputListener<TextInputTag,     void, const char*>              TextInputListener;
	typedef InputListener<MouseDownTag,     void, EMouseButton, uint, uint> MouseDownListener;
	typedef InputListener<MouseUpTag,       void, EMouseButton, uint, uint> MouseUpListener;
	typedef InputListener<MouseMovedTag,    void, uint, uint, int, int>     MouseMovedListener;
	typedef InputListener<MouseScrolledTag, void, int>                      MouseScrolledListener;
	typedef InputListener<WindowResizedTag, void, uint, uint>               WindowResizedListener;
	typedef InputListener<WindowQuitTag,    void>                           WindowQuitListener;

private:

#define DECLARE_LISTENER_TYPE(TYPE, LISTNAME) \
	friend class TYPE; \
	eastl::vector<TYPE*>& getListeners(const TYPE& a_listener) { return LISTNAME; } \
	eastl::vector<TYPE*> LISTNAME;

	DECLARE_LISTENER_TYPE(KeyDownListener, m_keyDownListeners);
	DECLARE_LISTENER_TYPE(KeyUpListener, m_keyUpListeners);
	DECLARE_LISTENER_TYPE(TextInputListener, m_textInputListeners);
	DECLARE_LISTENER_TYPE(MouseDownListener, m_mouseDownListeners);
	DECLARE_LISTENER_TYPE(MouseUpListener, m_mouseUpListeners);
	DECLARE_LISTENER_TYPE(MouseMovedListener, m_mouseMovedListeners);
	DECLARE_LISTENER_TYPE(MouseScrolledListener, m_mouseScrolledListeners);
	DECLARE_LISTENER_TYPE(WindowResizedListener, m_windowResizedListeners);
	DECLARE_LISTENER_TYPE(WindowQuitListener, m_windowQuitListeners);
#undef DECLARE_LISTENER_TYPE
};