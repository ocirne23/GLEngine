#pragma once

#include "rde\vector.h"

class KeyListener;
class MouseListener;
class WindowEventListener;

#include "Key.h"
#include "MouseButton.h"

class Input
{
public:
	Input();
	~Input();

	void registerKeyListener(KeyListener* listener);
	void registerMouseListener(MouseListener* listener);
	void unregisterKeyListener(KeyListener* listener);
	void unregisterMouseListener(MouseListener* listener);

	void keyDown(Key key);
	void keyUp(Key key);
	bool isKeyPressed(Key key);

	void mouseMoved(int xPos, int yPos, int deltaX, int deltaY);
	void mouseDown(MouseButton button, int xPos, int yPos);
	void mouseUp(MouseButton button, int xPos, int yPos);
	void mouseScrolled(int amount);

	void windowResize(int width, int height);
	void windowQuit();

private:

	rde::vector<KeyListener*> m_keyListeners;
	rde::vector<MouseListener*> m_mouseListeners;
	rde::vector<WindowEventListener*> m_windowEventListeners;
};