#include "Input/Input.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "Input/EKey.h"
#include "Input/EMouseButton.h"

#include <SDL/SDL_thread.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

void Input::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		m_eventQueue.push_back(*((Event*) &event));
}

void Input::processEvents()
{
	m_eventQueue.block();
	for (Event& e : m_eventQueue.getBackingQueue())
	{
		SDL_Event& event = (SDL_Event&) e;
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED: windowResized(event.window.data1, event.window.data2); break;
			case SDL_WINDOWEVENT_CLOSE:   windowQuit(); break;
			} break;
		case SDL_MOUSEMOTION:     mouseMoved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel); break;
		case SDL_MOUSEBUTTONDOWN: mouseDown((EMouseButton) event.button.button, event.button.x, event.button.y); break;
		case SDL_MOUSEBUTTONUP:   mouseUp((EMouseButton) event.button.button, event.button.x, event.button.y); break;
		case SDL_MOUSEWHEEL:      mouseScrolled(event.wheel.y); break;
		case SDL_KEYDOWN:         if (!event.key.repeat) keyDown((EKey) event.key.keysym.scancode); break;
		case SDL_KEYUP:           keyUp((EKey) event.key.keysym.scancode); break;
		case SDL_QUIT:            windowQuit(); break;
		}
	}
	m_eventQueue.getBackingQueue().clear();
	m_eventQueue.release();
}

void Input::setMouseCaptured(bool a_captured)
{
	SDL_SetRelativeMouseMode((SDL_bool) a_captured);
}

bool Input::isKeyPressed(EKey a_key)
{
	return SDL_GetKeyboardState(NULL)[(int) a_key] == 1;
}

bool Input::isMousePressed(EMouseButton a_button)
{
	return (SDL_BUTTON((int) a_button) & SDL_GetMouseState(NULL, NULL)) != 0;
}

void Input::keyDown(EKey a_key)
{
	for (KeyDownListener* l : m_keyDownListeners)
		l->call(a_key);
}

void Input::keyUp(EKey a_key)
{
	for (KeyUpListener* l : m_keyUpListeners)
		l->call(a_key);
}

void Input::mouseDown(EMouseButton button, uint xPos, uint yPos)
{
	for (MouseDownListener* l : m_mouseDownListeners)
		l->call(button, xPos, yPos);
}

void Input::mouseUp(EMouseButton button, uint xPos, uint yPos)
{
	for (MouseUpListener* l : m_mouseUpListeners)
		l->call(button, xPos, yPos);
}

void Input::mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY)
{
	for (MouseMovedListener* l : m_mouseMovedListeners)
		l->call(xPos, yPos, deltaX, deltaY);
}

void Input::mouseScrolled(int amount)
{
	for (MouseScrolledListener* l : m_mouseScrolledListeners)
		l->call(amount);
}

void Input::windowResized(uint width, uint height)
{
	for (WindowResizedListener* l : m_windowResizedListeners)
		l->call(width, height);
}

void Input::windowQuit()
{
	for (WindowQuitListener* l : m_windowQuitListeners)
		l->call();
}
