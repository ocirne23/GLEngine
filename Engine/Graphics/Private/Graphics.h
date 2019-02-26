#pragma once

#include "Core.h"
#include "Graphics/IGraphics.h"

class Graphics final : public IGraphics
{
public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) override;
	virtual void            destroyWindow(owner<IWindow*>& window) override;

private:

	virtual ~Graphics() override {}
};