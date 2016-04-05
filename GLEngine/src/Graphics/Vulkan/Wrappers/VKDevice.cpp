#include "Graphics/Vulkan/Wrappers/VKDevice.h"

#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "Graphics/Vulkan/Utils/VKDebug.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "EASTL/array.h"

VKDevice::~VKDevice()
{
	if (m_initialized)
		cleanup();
}

void VKDevice::initialize(VKPhysicalDevice& a_physDevice, EDeviceType a_type)
{
	if (m_initialized)
		cleanup();

	uint queueFamilyIndex = a_physDevice.getQueueNodeIndex(a_type);

	m_physDevice = &a_physDevice;
	m_type = a_type;

	eastl::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	eastl::array<float, 1> queuePriorities = { 0.0f };

	vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
		.queueFamilyIndex(queueFamilyIndex)
		.queueCount(1)
		.pQueuePriorities(queuePriorities.data());

	vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
		.queueCreateInfoCount(1)
		.pQueueCreateInfos(&deviceQueueCreateInfo);

	if (enabledExtensions.size())
	{
		deviceCreateInfo.enabledExtensionCount(uint(enabledExtensions.size()));
		deviceCreateInfo.ppEnabledExtensionNames(enabledExtensions.data());
	}
	if (VKDebug::VALIDATION_ENABLED)
	{
		deviceCreateInfo.enabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT);
		deviceCreateInfo.ppEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	m_device = a_physDevice.getVKPhysicalDevice().createDevice(deviceCreateInfo, NULL);
	
	m_queue = m_device.getQueue(queueFamilyIndex, 0);
	
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.queueFamilyIndex(queueFamilyIndex)
		.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	m_commandPool = m_device.createCommandPool(commandPoolCreateInfo, NULL);

	m_initialized = true;
}

void VKDevice::cleanup()
{
	assert(m_initialized);

	assert(false); //TODO

	m_initialized = false;
}
