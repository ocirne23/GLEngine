#include "Graphics.h"

owner<IGraphics*> Graphics::create()
{
	return owner<IGraphics*>();
}
