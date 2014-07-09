#include "Input\Input.h"

#include "Input\KeyListener.h"
#include "Input\MouseListener.h"

#include "Input\Key.h"
#include "Input\MouseButton.h"

#include <SDL\SDL_events.h>
#include <SDL\SDL_keyboard.h>

Input::Input()
{

}
Input::~Input()
{

}

void Input::setMouseCaptured(bool captured)
{
	SDL_SetRelativeMouseMode((SDL_bool) captured);
}

void Input::registerKeyListener(KeyListener* listener)
{
	m_keyListeners.push_back(listener);
}

void Input::registerMouseListener(MouseListener* listener)
{
	m_mouseListeners.push_back(listener);
}

void Input::unregisterKeyListener(KeyListener* listener)
{
	m_keyListeners.erase(m_keyListeners.find(listener));
}

void Input::unregisterMouseListener(MouseListener* listener)
{
	m_mouseListeners.erase(m_mouseListeners.find(listener));
}

void Input::keyDown(Key key)
{
	for (KeyListener* listener : m_keyListeners)
		if (listener->keyDown(key))
			break;
}

void Input::keyUp(Key key)
{
	for (KeyListener* listener : m_keyListeners)
		if (listener->keyUp(key))
			break;
}

bool Input::isKeyPressed(Key key)
{
	return SDL_GetKeyboardState(NULL)[key] == 1;
}

void Input::mouseMoved(int xPos, int yPos, int deltaX, int deltaY)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseMoved(xPos, yPos, deltaX, deltaY))
			break;
}

void Input::mouseDown(MouseButton button, int xPos, int yPos)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseDown(button, xPos, yPos))
			break;
}

void Input::mouseUp(MouseButton button, int xPos, int yPos)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseUp(button, xPos, yPos))
			break;
}

void Input::mouseScrolled(int amount)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseScrolled(amount))
			break;
}