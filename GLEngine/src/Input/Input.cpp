#include "Input/Input.h"

#include "Input/EKey.h"
#include "Input/EMouseButton.h"

#include <SDL/SDL_thread.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

void Input::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		m_eventQueue.push_back(*rcast<Input::Event*>(&event));
}

void Input::processEvents()
{
	m_eventQueue.block();
	for (const Event& e : m_eventQueue.getBackingQueue())
	{
		const SDL_Event& event = rcast<const SDL_Event&>(e);
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED: windowResized(event.window.data1, event.window.data2); break;
			case SDL_WINDOWEVENT_CLOSE:   windowQuit(); break;
			} break;
		case SDL_MOUSEMOTION:     mouseMoved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel); break;
		case SDL_MOUSEBUTTONDOWN: mouseDown(scast<EMouseButton>(event.button.button), event.button.x, event.button.y); break;
		case SDL_MOUSEBUTTONUP:   mouseUp(scast<EMouseButton>(event.button.button), event.button.x, event.button.y); break;
		case SDL_MOUSEWHEEL:      mouseScrolled(event.wheel.y); break;
		case SDL_KEYDOWN:         keyDown(scast<EKey>(event.key.keysym.scancode), event.key.repeat != 0); break;
		case SDL_KEYUP:           keyUp(scast<EKey>(event.key.keysym.scancode)); break;
		case SDL_TEXTINPUT:       textInput(event.text.text); break;
		case SDL_QUIT:            windowQuit(); break;
		}
	}
	m_eventQueue.getBackingQueue().clear();
	m_eventQueue.release();
}

void Input::setMouseCaptured(bool a_captured)
{
	SDL_SetRelativeMouseMode(scast<SDL_bool>(a_captured));
}

bool Input::isMouseCaptured()
{
	return SDL_GetRelativeMouseMode() == SDL_TRUE;
}

void Input::getMousePosition(int& a_xPos, int& a_yPos)
{
	SDL_GetMouseState(&a_xPos, &a_yPos);
}

bool Input::isKeyPressed(EKey a_key)
{
	return SDL_GetKeyboardState(NULL)[int(a_key)] == 1;
}

bool Input::isMousePressed(EMouseButton a_button)
{
	return (SDL_BUTTON(int(a_button)) & SDL_GetMouseState(NULL, NULL)) != 0;
}

void Input::keyDown(EKey a_key, bool a_isRepeat)
{
	for (auto l : m_keyDownListeners)
		l->call(a_key, a_isRepeat);
}

void Input::keyUp(EKey a_key)
{
	for (auto l : m_keyUpListeners)
		l->call(a_key);
}

void Input::mouseDown(EMouseButton a_button, uint a_xPos, uint a_yPos)
{
	for (auto l : m_mouseDownListeners)
		l->call(a_button, a_xPos, a_yPos);
}

void Input::mouseUp(EMouseButton a_button, uint a_xPos, uint a_yPos)
{
	for (auto l : m_mouseUpListeners)
		l->call(a_button, a_xPos, a_yPos);
}

void Input::mouseMoved(uint a_xPos, uint a_yPos, int a_deltaX, int a_deltaY)
{
	for (auto* l : m_mouseMovedListeners)
		l->call(a_xPos, a_yPos, a_deltaX, a_deltaY);
}

void Input::mouseScrolled(int a_amount)
{
	for (auto l : m_mouseScrolledListeners)
		l->call(a_amount);
}

void Input::windowResized(uint a_width, uint a_height)
{
	for (auto l : m_windowResizedListeners)
		l->call(a_width, a_height);
}

void Input::textInput(const char* a_text)
{
	for (auto l : m_textInputListeners)
		l->call(a_text);
}

void Input::windowQuit()
{
	for (auto l : m_windowQuitListeners)
		l->call();
}
