#include "App.h"

#include "Core/Utils.h"
#include "Graphics/GraphicsModule.h"
#include "Graphics/IGraphics.h"
#include "Graphics/IWindow.h"
#include "Graphics/IContext.h"
#include "Graphics/IBuffer.h"
#include "Graphics/IVertexAttributes.h"

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
	auto graphics = GraphicsModule::createGraphics();
	
	owner<IWindow*> window = graphics->createWindow("da", 640, 480, 700, 250, EWindowMode::WINDOWED);
	owner<IContext*> context = window->createContext(EContextType::OPENGL);
	owner<IBuffer*> vertexBuffer = context->createBuffer(EBufferType::VERTEX);
	owner<IBuffer*> indexBuffer = context->createBuffer(EBufferType::INDEX);
	owner<IVertexAttributes*> vertexAttributes = context->createVertexAttributes();
	owner<ICamera*> camera = graphics->createCamera();
	owner<IShader*> quadShader = context->createShader();
	// owner<IFragmentShaderStage*> fragmentStage = context->createFragmentShaderStageShader();
	// fragmentStage->setDefines({{"OUT_LOC", "1"}, {"

	vertexBuffer->initialize(sizeof(quad), EBufferUsageType::STATIC);
	indexBuffer->initialize(sizeof(indices), EBufferUsageType::STATIC);

	vertexBuffer->upload(make_span(rcast<const byte*>(quad), sizeof(quad)));
	indexBuffer->upload(make_span(rcast<const byte*>(indices), sizeof(indices)));

	vertexAttributes->addVertexAttribute(EVertexAttributeFormat::FLOAT, 3);
	vertexAttributes->addVertexAttribute(EVertexAttributeFormat::FLOAT, 2);
	vertexAttributes->bind(*vertexBuffer);

	window->swapBuffer();
	std::cin.get();

	context->destroyShader(quadShader);
	graphics->destroyCamera(camera);
	context->destroyVertexAttributes(vertexAttributes);
	context->destroyBuffer(indexBuffer);
	context->destroyBuffer(vertexBuffer);
	window->destroyContext(context);
	graphics->destroyWindow(window);

	GraphicsModule::destroyGraphics(graphics);
	
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