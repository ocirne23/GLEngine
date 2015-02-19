#include "Input/Input.h"

#include "Input/Key.h"
#include "Input/MouseButton.h"

#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

void Input::setMouseCaptured(bool a_captured)
{
	SDL_SetRelativeMouseMode((SDL_bool) a_captured);
}

void Input::keyDown(Key a_key)
{
	for (auto& listener : m_keyDownListeners)
		if (listener.second(a_key))
			break;
}

void Input::keyUp(Key a_key)
{
	for (auto& listener : m_keyUpListeners)
		if (listener.second(a_key))
			break;
}

void Input::mouseMoved(uint a_xPos, uint a_yPos, int a_deltaX, int a_deltaY)
{
	for (auto& listener : m_mouseMovedListeners)
		if (listener.second(a_xPos, a_yPos, a_deltaX, a_deltaY))
			break;
}

void Input::mouseDown(MouseButton a_button, int a_xPos, int a_yPos)
{
	for (auto& listener : m_mouseDownListeners)
		if (listener.second(a_button, a_xPos, a_yPos))
			break;
}

void Input::mouseUp(MouseButton a_button, int a_xPos, int a_yPos)
{
	for (auto& listener : m_mouseUpListeners)
		if (listener.second(a_button, a_xPos, a_yPos))
			break;
}

void Input::mouseScrolled(int a_amount)
{
	for (auto& listener : m_mouseScrolledListeners)
		if (listener.second(a_amount))
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