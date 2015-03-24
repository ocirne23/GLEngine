#pragma once

#include "Core.h"

#include "Input/EKey.h"
#include "Input/EMouseButton.h"

#include "Utils/ListenerMacros.h"
#include "Utils/ConcurrentQueue.h"

struct Event
{
	byte data[56];
};

class Input
{
public:
	friend class GLEngine;

	bool isKeyPressed(EKey key);
	bool isMousePressed(EMouseButton button);
	void setMouseCaptured(bool captured);

	DECLARE_LISTENER_CONSUME(keyDown, EKey);
	DECLARE_LISTENER_CONSUME(keyUp, EKey);
	DECLARE_LISTENER_CONSUME(mouseDown, EMouseButton, int, int);
	DECLARE_LISTENER_CONSUME(mouseUp, EMouseButton, int, int);
	DECLARE_LISTENER_CONSUME(mouseMoved, uint, uint, int, int)
	DECLARE_LISTENER_CONSUME(mouseScrolled, int);
	DECLARE_LISTENER(windowResized, uint, uint);
	DECLARE_LISTENER(windowQuit);

private:

	Input() {}
	~Input() {}

	void pollEvents();
	void processEvents();

	void keyDown(EKey key)                                        { CALL_LISTENERS_CONSUME(keyDown, key) }
	void keyUp(EKey key)                                          { CALL_LISTENERS_CONSUME(keyUp, key) }
	void mouseDown(EMouseButton button, int xPos, int yPos)       { CALL_LISTENERS_CONSUME(mouseDown, button, xPos, yPos) }
	void mouseUp(EMouseButton button, int xPos, int yPos)         { CALL_LISTENERS_CONSUME(mouseUp, button, xPos, yPos) }
	void mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY) { CALL_LISTENERS_CONSUME(mouseMoved, xPos, yPos, deltaX, deltaY) }
	void mouseScrolled(int amount)                                { CALL_LISTENERS_CONSUME(mouseScrolled, amount) }
	void windowResized(uint width, uint height)                   { CALL_LISTENERS(windowResized, width, height) }
	void windowQuit()                                             { CALL_LISTENERS(windowQuit) }

private:

	ConcurrentQueue<Event> m_eventQueue;
};