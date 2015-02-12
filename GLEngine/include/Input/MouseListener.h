#pragma once

#include "Input/MouseButton.h"

class MouseListener
{
public:
	virtual ~MouseListener() {}

	virtual bool mouseDown(MouseButton /*key*/, int /*xPos*/, int /*yPos*/)				{ return false; }
	virtual bool mouseUp(MouseButton /*key*/, int /*xPos*/, int /*yPos*/)				{ return false; }
	virtual bool mouseMoved(int /*xPos*/, int /*yPos*/, int /*xMove*/, int /*yMove*/)	{ return false; }
	virtual bool mouseScrolled(int /*amount*/)											{ return false; }
};