#include "Graphics.h"

owner<IGraphics*> IGraphics::create()
{
	return new Graphics();
}