#pragma once

enum Key;

class KeyListener
{
public:
	virtual ~KeyListener() {}

	virtual bool keyDown(Key /*key*/)
	{
		return false;
	}
	virtual bool keyUp(Key /*key*/) 
	{ 
		return false; 
	}
};