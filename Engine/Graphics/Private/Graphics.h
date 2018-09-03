#pragma once

#include "Core.h"
#include "Graphics/IGraphics.h"

class IWindow;
enum class EWindowMode;

class ICamera;

class Graphics : public IGraphics
{
public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) override;
	virtual void destroyWindow(owner<IWindow*> window) override;

	virtual owner<ICamera*> createCamera() override;
	virtual void destroyCamera(owner<ICamera*> camera) override;

private:

	friend class GraphicsModule;
	Graphics() {}
	Graphics(const Graphics&) = delete;
	virtual ~Graphics() override;

#ifdef DEBUG_BUILD
private:
	int md_numWindowsCreated = 0;
	int md_numCamerasCreated = 0;
#endif
};