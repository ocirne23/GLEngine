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

	eastl::vector<KeyDownListener*>& getListeners(const KeyDownListener& a_listener) { return m_keyDownListeners; }
	eastl::vector<KeyUpListener*>& getListeners(const KeyUpListener& a_listener) { return m_keyUpListeners; }
	eastl::vector<TextInputListener*>& getListeners(const TextInputListener& a_listener) { return m_textInputListeners; }
	eastl::vector<MouseDownListener*>& getListeners(const MouseDownListener& a_listener) { return m_mouseDownListeners; }
	eastl::vector<MouseUpListener*>& getListeners(const MouseUpListener& a_listener) { return m_mouseUpListeners; }
	eastl::vector<MouseMovedListener*>& getListeners(const MouseMovedListener& a_listener) { return m_mouseMovedListeners; }
	eastl::vector<MouseScrolledListener*>& getListeners(const MouseScrolledListener& a_listener) { return m_mouseScrolledListeners; }
	eastl::vector<WindowResizedListener*>& getListeners(const WindowResizedListener& a_listener) { return m_windowResizedListeners; }
	eastl::vector<WindowQuitListener*>& getListeners(const WindowQuitListener& a_listener) { return m_windowQuitListeners; }

private:

	eastl::vector<KeyDownListener*> m_keyDownListeners;
	eastl::vector<KeyUpListener*> m_keyUpListeners;
	eastl::vector<TextInputListener*> m_textInputListeners;
	eastl::vector<MouseDownListener*> m_mouseDownListeners;
	eastl::vector<MouseUpListener*> m_mouseUpListeners;
	eastl::vector<MouseMovedListener*> m_mouseMovedListeners;
	eastl::vector<MouseScrolledListener*> m_mouseScrolledListeners;
	eastl::vector<WindowResizedListener*> m_windowResizedListeners;
	eastl::vector<WindowQuitListener*> m_windowQuitListeners;
};