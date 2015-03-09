#include "Input/Input.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "Input/Key.h"
#include "Input/MouseButton.h"

#include <SDL/SDL_thread.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

void Input::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		m_eventQueue.push_back(*((Event*) &event));
	}
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
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				windowResized(event.window.data1, event.window.data2);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				windowQuit();
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
			if (!event.key.repeat)
				keyDown((Key) event.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			keyUp((Key) event.key.keysym.scancode);
			break;
		case SDL_QUIT:
			windowQuit();
			break;
		}
	}
	m_eventQueue.getBackingQueue().clear();
	m_eventQueue.release();
}

void Input::setMouseCaptured(bool a_captured)
{
	SDL_SetRelativeMouseMode((SDL_bool) a_captured);
}

bool Input::isKeyPressed(Key a_key)
{
	return SDL_GetKeyboardState(NULL)[a_key] == 1;
}

bool Input::isMousePressed(MouseButton a_button)
{
	return (SDL_BUTTON(a_button) & SDL_GetMouseState(NULL, NULL)) != 0;
}