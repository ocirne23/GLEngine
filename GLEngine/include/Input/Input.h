#pragma once

#include "Core.h"

#include "Key.h"
#include "MouseButton.h"

#include "Utils/Listener.h"

class Input
{
public:
	friend class GLEngine;

	bool isKeyPressed(Key key);
	bool isMousePressed(MouseButton button);
	void setMouseCaptured(bool captured);

	DECLARE_LISTENER_H(keyDown, bool, Key);
	DECLARE_LISTENER_H(keyUp, bool, Key);
	DECLARE_LISTENER_H(mouseDown, bool, MouseButton, int, int);
	DECLARE_LISTENER_H(mouseUp, bool, MouseButton, int, int);
	DECLARE_LISTENER_H(mouseMoved, bool, uint, uint, int, int)
	DECLARE_LISTENER_H(mouseScrolled, bool, int);

private:

	void keyDown(Key key);
	void keyUp(Key key);

	void mouseDown(MouseButton button, int xPos, int yPos);
	void mouseUp(MouseButton button, int xPos, int yPos);
	void mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY);
	void mouseScrolled(int amount);
};