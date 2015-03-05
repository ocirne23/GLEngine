#pragma once

#include "Core.h"

#include "Key.h"
#include "MouseButton.h"

#include "Utils/ListenerMacros.h"
#include "Utils/ConcurrentQueue.h"

struct SDL_Thread;

class Input
{
public:
	Input();
	~Input();

	void processEvents();

	bool isKeyPressed(Key key);
	bool isMousePressed(MouseButton button);
	void setMouseCaptured(bool captured);

	DECLARE_LISTENER(keyDown, bool, Key);
	DECLARE_LISTENER(keyUp, bool, Key);
	DECLARE_LISTENER(mouseDown, bool, MouseButton, int, int);
	DECLARE_LISTENER(mouseUp, bool, MouseButton, int, int);
	DECLARE_LISTENER(mouseMoved, bool, uint, uint, int, int)
	DECLARE_LISTENER(mouseScrolled, bool, int);

private:

	void keyDown(Key key);
	void keyUp(Key key);

	void mouseDown(MouseButton button, int xPos, int yPos);
	void mouseUp(MouseButton button, int xPos, int yPos);
	void mouseMoved(uint xPos, uint yPos, int deltaX, int deltaY);
	void mouseScrolled(int amount);

private:

	static int inputThread(void* ptr);

private:

	struct Event
	{
		byte padding[56];
	};

	ConcurrentQueue<Event> m_eventQueue;
	SDL_Thread* m_inputThread;
	bool m_inputThreadRunning = true;
	bool m_inputThreadHasExited = false;
};