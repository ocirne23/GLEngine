#include "Graphics/Vulkan/VKApplication.h"

#include "EASTL/vector.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Graphics/Vulkan/Utils/VKDebug.h"
#include "gsl/gsl.h"

BEGIN_UNNAMED_NAMESPACE()

END_UNNAMED_NAMESPACE()

void VKApplication::initialize()
{

}

vk::Result VKApplication::createInstance(bool enableValidation)
{
	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.apiVersion(VK_API_VERSION)
		.engineVersion(0)
		.pEngineName("GLEngine")
		.applicationVersion(0)
		.pApplicationName("GLApp");

	eastl::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };


	vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
		.pApplicationInfo(&appInfo);

	if (enabledExtensions.size() > 0)
	{
		if (enableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		instanceCreateInfo.enabledExtensionCount((uint32_t)enabledExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames(enabledExtensions.data());
	}
	if (enableValidation)
	{
		instanceCreateInfo.enabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT); // todo : change validation layer names!
		instanceCreateInfo.ppEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	return vk::createInstance(&instanceCreateInfo, NULL, &m_instance);
}

vk::Result VKApplication::createDevice(vk::DeviceQueueCreateInfo requestedQueues, bool enableValidation)
{
	return vk::Result();
}
