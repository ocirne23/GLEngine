#include "Input/Input.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "Input/Key.h"
#include "Input/MouseButton.h"

#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

void Input::processEvents()
{
	m_eventQueue.block();
	for (Event i : m_eventQueue.getBackingQueue())
	{
		SDL_Event& event = *(SDL_Event*) &i;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					GLEngine::graphics->windowResize(event.window.data1, event.window.data2);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				mouseMoved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown((MouseButton) event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				mouseUp((MouseButton) event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEWHEEL:
				mouseScrolled(event.wheel.y);
				break;
			case SDL_KEYDOWN:
				keyDown((Key) event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				keyUp((Key) event.key.keysym.scancode);
				break;
			case SDL_QUIT:
				GLEngine::graphics->windowQuit();
				break;
			}
		}

	}
}

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