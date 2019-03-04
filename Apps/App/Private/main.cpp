#include "App.h"

#include "Core/Utils.h"
#include "Graphics/IGraphics.h"
#include "Graphics/IWindow.h"
#include "Graphics/IContext.h"
#include "Graphics/ITexture.h"

#include <iostream>
#include <stdio.h>

BEGIN_UNNAMED_NAMESPACE()

const float quad[] =
{// Position				Texcoords
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
};

const uint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

END_UNNAMED_NAMESPACE()

int main()
{
	owner<IGraphics*> graphics = IGraphics::createGraphics();
	owner<IWindow*> window = graphics->createWindow("da", 512, 512, 1920 / 2 - 256, 1080 / 2 - 256, EWindowMode::WINDOWED);
	owner<IContext*> context = window->createContext(EContextType::OPENGL);
	owner<ITexture*> texture = context->createTexture(ETextureType::TEX2D);
	texture->setOptions(TextureSettings());
	texture->loadTexture(nullptr, ETexturePixelDataFormat::BYTE);

	printf("Ello\n");
	window->swapBuffers();
	std::cin.get();

	context->destroyTexture(texture);
	window->destroyContext(context);
	graphics->destroyWindow(window);
	IGraphics::destroyGraphics(graphics);
	return 0;
}

// EASTL new defines
#if defined(LIB_BUILD)
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return new char[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return operator new[](size, pName, flags, debugFlags, file, line);
}
#endif