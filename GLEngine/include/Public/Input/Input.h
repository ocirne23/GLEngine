#pragma once

#include "Core.h"

#include "Input/EKey.h"
#include "Input/EMouseButton.h"

#include "Utils/ConcurrentQueue.h"
#include "Input/InputListener.h"
#include "3rdparty/rde/vector.h"

class Input
{
public:

	struct Event
	{
		byte data[56];
	};

public:
	friend class GLEngine;

	bool isKeyPressed(EKey key);
	bool isMousePressed(EMouseButton button);
	void setMouseCaptured(bool captured);

private:

	Input() {}
	~Input() {}

	void pollEvents();
	void processEvents();

	void keyDown(EKey key);
	void keyUp(EKey key);
	void mouseDown(EMouseButton button, uint xPos, uint yPos);
	void mouseUp(EMouseButton button, uint xPos, uint yPos);
	void mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY);
	void mouseScrolled(int amount);
	void windowResized(uint width, uint height);
	void windowQuit();

private:
	ConcurrentQueue<Event> m_eventQueue;

public:
	// Listener stuff from here on //

	struct KeyDownTag {};
	struct KeyUpTag {};
	struct MouseDownTag {};
	struct MouseUpTag {};
	struct MouseMovedTag {};
	struct MouseScrolledTag {};
	struct WindowResizedTag {};
	struct WindowQuitTag {};

	typedef InputListener<KeyDownTag, void, EKey> KeyDownListener;
	typedef InputListener<KeyUpTag, void, EKey> KeyUpListener;
	typedef InputListener<MouseDownTag, void, EMouseButton, uint, uint> MouseDownListener;
	typedef InputListener<MouseUpTag, void, EMouseButton, uint, uint> MouseUpListener;
	typedef InputListener<MouseMovedTag, void, uint, uint, int, int> MouseMovedListener;
	typedef InputListener<MouseScrolledTag, void, int> MouseScrolledListener;
	typedef InputListener<WindowResizedTag, void, uint, uint> WindowResizedListener;
	typedef InputListener<WindowQuitTag, void> WindowQuitListener;

private:

#define LISTENERS_GET(TYPE, LISTNAME) \
	friend class TYPE; \
	rde::vector<TYPE*>& getListeners(const TYPE& a_listener) { return LISTNAME;} \
	rde::vector<TYPE*> LISTNAME;

	LISTENERS_GET(KeyDownListener, m_keyDownListeners);
	LISTENERS_GET(KeyUpListener, m_keyUpListeners);
	LISTENERS_GET(MouseDownListener, m_mouseDownListeners);
	LISTENERS_GET(MouseUpListener, m_mouseUpListeners);
	LISTENERS_GET(MouseMovedListener, m_mouseMovedListeners);
	LISTENERS_GET(MouseScrolledListener, m_mouseScrolledListeners);
	LISTENERS_GET(WindowResizedListener, m_windowResizedListeners);
	LISTENERS_GET(WindowQuitListener, m_windowQuitListeners);

#undef LISTENERS_GET
};