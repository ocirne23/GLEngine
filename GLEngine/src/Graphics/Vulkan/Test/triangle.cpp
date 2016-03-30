#include "Graphics/Vulkan/Test/triangle.h"

#include "Core.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Test/vulkanexamplebase.h"
#include <Windows.h>

void triangleTest()
{
	VulkanExampleBase* vulkanExample = new VulkanExampleBase(false);
	vulkanExample->setupWindow(
		rcast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()),
		rcast<HWND>(GLEngine::graphics->getHWND()));
	vulkanExample->initSwapchain();
	vulkanExample->prepare();
	vulkanExample->renderLoop();
	delete(vulkanExample);
}
