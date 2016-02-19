/*
#include "Graphics/Vulkan/VKApplication.h"

#include "EASTL/vector.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "gsl/gsl.h"

BEGIN_UNNAMED_NAMESPACE()

const char* INSTANCE_VALIDATION_LAYERS[] = { "VK_LAYER_LUNARG_mem_tracker", "VK_LAYER_GOOGLE_unique_objects" };
const char* INSTANCE_EXTENTION_NAMES[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
const char* DEVICE_EXTENTION_NAMES[] = { "VK_KHR_swapchain" };

void verifyInstance(span<const char*> a_instanceValidationLayerNames, span<const char*> a_instanceExtensionNames)
{
	eastl::vector<vk::LayerProperties> instanceLayerProperties;
	VKVerifier result = vk::enumerateInstanceLayerProperties(instanceLayerProperties);
	for (uint i = 0; i < a_instanceValidationLayerNames.size(); ++i)
	{
		bool found = false;
		for (uint j = 0; j < instanceLayerProperties.size(); ++j)
			if (!strcmp(instanceLayerProperties[j].layerName(), a_instanceValidationLayerNames[i]))
			{
				found = true;
				break;
			}

		if (!found)
		{
			print("vkEnumerateInstanceLayerProperties failed to find required validation layer: %s\n", a_instanceValidationLayerNames[i]);
			break;
		}
	}

	eastl::vector<vk::ExtensionProperties> instanceExtensionProperties;
	result = vk::enumerateInstanceExtensionProperties(eastl::string(), instanceExtensionProperties);
	for (uint i = 0; i < a_instanceExtensionNames.size(); ++i)
	{
		bool found = false;
		for (uint j = 0; j < instanceExtensionProperties.size(); ++j)
			if (!strcmp(instanceExtensionProperties[j].extensionName(), a_instanceExtensionNames[i]))
			{
				found = true;
				break;
			}
		if (!found)
		{
			print("enumerateInstanceExtensionProperties failed to find required extension: %s\n", a_instanceExtensionNames[i]);
			break;
		}
	}
}

vk::Instance createInstance()
{
	verifyInstance(as_span(INSTANCE_VALIDATION_LAYERS), as_span(INSTANCE_EXTENTION_NAMES));

	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.apiVersion(VK_API_VERSION)
		.engineVersion(0)
		.pEngineName("GLEngine")
		.applicationVersion(0)
		.pApplicationName("GLApp");

	vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
		.pApplicationInfo(&appInfo)
		.enabledExtensionCount(ARRAY_SIZE(INSTANCE_EXTENTION_NAMES))
		.ppEnabledExtensionNames(INSTANCE_EXTENTION_NAMES)
		.enabledLayerCount(ARRAY_SIZE(INSTANCE_VALIDATION_LAYERS))
		.ppEnabledLayerNames(INSTANCE_VALIDATION_LAYERS);

	vk::Instance instance;
	VKVerifier result = vk::createInstance(&instanceCreateInfo, NULL, &instance);
	return instance;
}

vk::PhysicalDevice findPhysicalDevice(vk::Instance a_instance)
{
	eastl::vector<vk::PhysicalDevice> physDevices;
	VKVerifier result = vk::enumeratePhysicalDevices(a_instance, physDevices);
	if (physDevices.empty())
	{
		print("No Vulkan compatible devices found\n");
		return VK_NULL_HANDLE;
	}
	else
	{
		print("Num Vulkan compatible devices: %i\n", physDevices.size());
	}
	return physDevices[0];
}

vk::SurfaceKHR createSurface(vk::Instance a_instance)
{
	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.hinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.hwnd(scast<HWND>(GLEngine::graphics->getHWND()));

	vk::SurfaceKHR surface;
	VKVerifier result = vk::createWin32SurfaceKHR(a_instance, &win32SurfaceCreateInfo, NULL, &surface);
	return surface;
}

uint findGraphicsQueueIndex(vk::PhysicalDevice a_physDevice, vk::SurfaceKHR a_surface)
{
	eastl::vector<vk::QueueFamilyProperties> queueFamilyProperties = vk::getPhysicalDeviceQueueFamilyProperties(a_physDevice);
	eastl::vector<VkBool32> supportsPresent;
	supportsPresent.resize(queueFamilyProperties.size());
	for (uint i = 0; i < queueFamilyProperties.size(); ++i)
	{
		VKVerifier result = vk::getPhysicalDeviceSurfaceSupportKHR(a_physDevice, i, a_surface, supportsPresent[i]);
	}

	// Search for a graphics and a present queue in the array of queue families, try to find one that supports both
	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (uint i = 0; i < queueFamilyProperties.size(); ++i)
	{
		if ((queueFamilyProperties[i].queueFlags() & vk::QueueFlagBits::eGraphics) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX) {
				graphicsQueueNodeIndex = i;
			}
			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX)
	{// If didn't find a queue that supports both graphics and present, then find a separate present queue.
		for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
		{
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
	{// Generate error if could not find both a graphics and a present queue
		print("Could not find a graphics and a present queue\n");
		assert(false);
	}
	if (graphicsQueueNodeIndex != presentQueueNodeIndex)
	{// TODO: Add support for separate queues
		print("Could not find a common graphics and a present queue\n");
		assert(false);
	}

	return graphicsQueueNodeIndex;
}

vk::Device createDevice(vk::PhysicalDevice a_physDevice, uint a_graphicsQueueIndex)
{
	float queue_priorities[1] = { 0.0 };
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
		.queueCount(1)
		.queueFamilyIndex(a_graphicsQueueIndex)
		.pQueuePriorities(queue_priorities);

	vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
		.queueCreateInfoCount(1)
		.pQueueCreateInfos(&deviceQueueCreateInfo)
		.enabledLayerCount(ARRAY_SIZE(INSTANCE_VALIDATION_LAYERS))
		.ppEnabledLayerNames(INSTANCE_VALIDATION_LAYERS)
		.enabledExtensionCount(ARRAY_SIZE(DEVICE_EXTENTION_NAMES))
		.ppEnabledExtensionNames(DEVICE_EXTENTION_NAMES);

	vk::Device device;
	VKVerifier result = vk::createDevice(a_physDevice, &deviceCreateInfo, NULL, &device);
	return device;
}

vk::Queue getDeviceQueue(vk::Device a_device, uint a_graphicsQueuindex)
{
	vk::Queue queue;
	vk::getDeviceQueue(a_device, a_graphicsQueuindex, 0, queue);
	return queue;
}

void printPhysicalDeviceProperties(vk::PhysicalDevice a_physDevice)
{
	vk::PhysicalDeviceProperties physicalDeviceProperties;
	vk::getPhysicalDeviceProperties(a_physDevice, physicalDeviceProperties);
	const vk::PhysicalDeviceLimits& limits = physicalDeviceProperties.limits();
	print("Device: %s, driver version: %i, api version: %i\n",
		physicalDeviceProperties.deviceName(),
		physicalDeviceProperties.driverVersion(),
		physicalDeviceProperties.apiVersion());
	print("Max memory allocation: %i\nMax image dimensions: %i\n",
		limits.maxMemoryAllocationCount(),
		limits.maxImageDimension2D());
#if PRINT_LAYER_PROPERTIES
	eastl::vector<vk::LayerProperties> layerProperties;
	VK_VERIFY(vk::enumerateDeviceLayerProperties(physDevice, layerProperties));
	for (const auto& layerProperty : layerProperties)
	{
		print("name: %s, desc: %s, specVersion: %i, implVersion: %i\n",
			layerProperty.layerName(),
			layerProperty.description(),
			layerProperty.specVersion(),
			layerProperty.implementationVersion());
	}
#endif
}

vk::CommandPool createCommandPool(vk::Device a_device, uint a_graphicsQueueIndex)
{
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.queueFamilyIndex(a_graphicsQueueIndex)
		.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	vk::CommandPool pool;
	VKVerifier result = vk::createCommandPool(a_device, &commandPoolCreateInfo, NULL, &pool);
	return pool;
}

vk::CommandBuffer allocateCommandBuffer(vk::Device a_device, vk::CommandPool a_pool)
{
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.commandPool(a_pool)
		.level(vk::CommandBufferLevel::ePrimary)
		.commandBufferCount(1);

	eastl::vector<vk::CommandBuffer> commandBuffers;
	commandBuffers.resize(1);
	vk::CommandBuffer commandBuffer;
	VKVerifier result = vk::allocateCommandBuffers(a_device, &commandBufferAllocateInfo, &commandBuffer);
	return commandBuffer;
}

vk::SwapchainKHR createSwapchain(vk::PhysicalDevice a_physDevice, vk::Device a_device, vk::SurfaceKHR a_surface)
{
	vk::Format format;
	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats;
	VKVerifier result = vk::getPhysicalDeviceSurfaceFormatsKHR(a_physDevice, a_surface, surfaceFormats);
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format() == vk::Format::eUndefined)
	{
		format = vk::Format::eB8G8R8A8Unorm;
	}
	else
	{
		assert(surfaceFormats.size() > 1);
		format = surfaceFormats[0].format();
	}
	vk::ColorSpaceKHR colorSpace = surfaceFormats[0].colorSpace();

	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	result = vk::getPhysicalDeviceSurfaceCapabilitiesKHR(a_physDevice, a_surface, surfaceCapabilities);

	eastl::vector<vk::PresentModeKHR> presentModes;
	result = vk::getPhysicalDeviceSurfacePresentModesKHR(a_physDevice, a_surface, presentModes);

	vk::Extent2D swapchainExtent(surfaceCapabilities.currentExtent());
	if (surfaceCapabilities.currentExtent().width() == (uint)-1)
	{
		swapchainExtent.width(GLEngine::graphics->getViewportWidth());
		swapchainExtent.height(GLEngine::graphics->getViewportHeight());
	}

	vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeFifoKhr;
	uint desiredNumSwapChainImages = surfaceCapabilities.minImageCount() + 1;
	if (surfaceCapabilities.maxImageCount() > 0 && desiredNumSwapChainImages > surfaceCapabilities.maxImageCount())
		desiredNumSwapChainImages = surfaceCapabilities.maxImageCount();

	vk::SurfaceTransformFlagBitsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms() & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	else
		preTransform = surfaceCapabilities.currentTransform();

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
		.surface(a_surface)
		.minImageCount(desiredNumSwapChainImages)
		.imageFormat(format)
		.imageColorSpace(colorSpace)
		.imageExtent(swapchainExtent)
		.imageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.preTransform(preTransform)
		.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.imageArrayLayers(1)
		.imageSharingMode(vk::SharingMode::eExclusive)
		.queueFamilyIndexCount(0)
		.pQueueFamilyIndices(NULL)
		.presentMode(swapchainPresentMode)
		.oldSwapchain(NULL)
		.clipped(VK_TRUE);

	vk::SwapchainKHR swapchain;
	result = vk::createSwapchainKHR(a_device, &swapchainCreateInfo, NULL, &swapchain);
	return swapchain;
}

void setImageLayout()
{

}

void setupSwapchainBuffers()
{
	eastl::vector<vk::Image> swapchainImages;
	vk::getSwapchainImagesKHR(device, swapchain, swapchainImages);
	for (uint i = 0; i < swapchainImages.size(); ++i)
	{
		vk::ImageViewCreateInfo colorAttachmentViewCreateInfo = vk::ImageViewCreateInfo()
			.format(format)
			.components(vk::ComponentMapping(
				vk::ComponentSwizzle::eR,  // r
				vk::ComponentSwizzle::eG,  // g
				vk::ComponentSwizzle::eB,  // b
				vk::ComponentSwizzle::eA)) // a
			.subresourceRange(vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor, // aspectMask
				0,	// baseMipLevel
				1,  // levelCount
				0,  // baseArrayLayer
				1)) // layerCount
			.viewType(vk::ImageViewType::e2D);


		//demo_set_image_layout(
		//	demo, demo->buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT,
		//	VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	}

	END_UNNAMED_NAMESPACE()

		void VKApplication::initialize()
	{
		m_instance = createInstance();
		m_physDevice = findPhysicalDevice(m_instance);
		printPhysicalDeviceProperties(m_physDevice);
		m_surface = createSurface(m_instance);
		uint graphicsQueueIndex = findGraphicsQueueIndex(m_physDevice, m_surface);
		m_device = createDevice(m_physDevice, graphicsQueueIndex);
		m_queue = getDeviceQueue(m_device, graphicsQueueIndex);
		m_commandPool = createCommandPool(m_device, graphicsQueueIndex);
		m_commandBuffers[uint(ECommandBuffers::Test1)] = allocateCommandBuffer(m_device, m_commandPool);
		m_swapchain = createSwapchain(m_physDevice, m_device, m_surface);
		print("end\n");
	}


*/