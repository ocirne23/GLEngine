#pragma once

#include "Core.h"

class IContext;
enum class EContextType;

enum class EWindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	NONE
};

class IWindow
{
public:

	virtual owner<IContext*> createContext(const EContextType& type) = 0;
	virtual void destroyContext(owner<IContext*> context) = 0;
	virtual void swapBuffer() = 0;

protected:

	friend class Graphics;
	virtual ~IWindow() {}
};