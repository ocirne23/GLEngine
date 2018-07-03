#pragma once

#include "Core.h"

class IWindow;
class IContext;
enum class EWindowMode;
enum class EContextType;

class Graphics
{
public:

	Graphics() {}
	Graphics(const Graphics&) = delete;
	~Graphics() {}

	owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);
	owner<IContext> createContext(const EContextType& type);
};