#pragma once

#include "Core.h"

class IWindow;
enum class EWindowMode;
class ICamera;

class IGraphics
{
public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) = 0;
	virtual void destroyWindow(owner<IWindow*> window) = 0;

	virtual owner<ICamera*> createCamera() = 0;
	virtual void destroyCamera(owner<ICamera*> camera) = 0;

protected:

	friend class GraphicsModule;
	virtual ~IGraphics() {}
};