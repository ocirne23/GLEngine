#include "Graphics/GraphicsModule.h"

#include "Graphics.h"

owner<IGraphics*> GraphicsModule::createGraphics()
{
	return owner<IGraphics*>(new Graphics());
}

void GraphicsModule::destroyGraphics(owner<IGraphics*> graphics)
{
	assert(graphics);
	delete static_cast<IGraphics*>(graphics);
}
