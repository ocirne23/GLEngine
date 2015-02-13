#include "Input/Input.h"

#include "Input/KeyListener.h"
#include "Input/MouseListener.h"
#include "Input/Key.h"
#include "Input/MouseButton.h"

#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

Input::Input()
{

}
Input::~Input()
{

}

void Input::setMouseCaptured(bool a_captured)
{
	SDL_SetRelativeMouseMode((SDL_bool) a_captured);
}

void Input::registerKeyListener(KeyListener* a_listener)
{
	m_keyListeners.push_back(a_listener);
}

void Input::registerMouseListener(MouseListener* a_listener)
{
	m_mouseListeners.push_back(a_listener);
}

void Input::unregisterKeyListener(KeyListener* a_listener)
{
	m_keyListeners.erase(m_keyListeners.find(a_listener));
}

void Input::unregisterMouseListener(MouseListener* a_listener)
{
	m_mouseListeners.erase(m_mouseListeners.find(a_listener));
}

void Input::keyDown(Key a_key)
{
	for (KeyListener* listener : m_keyListeners)
		if (listener->keyDown(a_key))
			break;
}

void Input::keyUp(Key a_key)
{
	for (KeyListener* listener : m_keyListeners)
		if (listener->keyUp(a_key))
			break;
}

bool Input::isKeyPressed(Key a_key)
{
	return SDL_GetKeyboardState(NULL)[a_key] == 1;
}

bool Input::isMousePressed(MouseButton a_button)
{
	return (SDL_BUTTON(a_button) & SDL_GetMouseState(NULL, NULL)) != 0;
}

void Input::mouseMoved(int a_xPos, int a_yPos, int a_deltaX, int a_deltaY)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseMoved(a_xPos, a_yPos, a_deltaX, a_deltaY))
			break;
}

void Input::mouseDown(MouseButton a_button, int a_xPos, int a_yPos)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseDown(a_button, a_xPos, a_yPos))
			break;
}

void Input::mouseUp(MouseButton a_button, int a_xPos, int a_yPos)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseUp(a_button, a_xPos, a_yPos))
			break;
}

void Input::mouseScrolled(int a_amount)
{
	for (MouseListener* listener : m_mouseListeners)
		if (listener->mouseScrolled(a_amount))
			break;
}