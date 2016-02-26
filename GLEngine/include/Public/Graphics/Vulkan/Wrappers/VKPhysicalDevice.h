#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"

#include "gsl/gsl.h"
#include "EASTL/vector.h"

class VKInstance;
class VKSwapchain;

class VKPhysicalDevice
{
public:

	~VKPhysicalDevice();

	bool isInitialized() const { return m_initialized; }

	VKDevice& getDevice(VKDevice::EDeviceType deviceType = VKDevice::EDeviceType::Graphics);
	VKSwapchain& getSwapchain();
	uint getQueueNodeIndex(VKDevice::EDeviceType deviceType);
	vk::Format getDepthFormat();

	VKInstance* getInstance() 
	{ 
		assert(m_initialized); 
		return m_instance; 
	}
	vk::PhysicalDevice getVKPhysicalDevice()
	{
		assert(m_initialized);
		return m_physDevice;
	}
	const eastl::vector<vk::QueueFamilyProperties>& getQueueFamilyProperties() const
	{ 
		assert(m_initialized);
		return m_queueFamilyProperties; 
	}
	const vk::PhysicalDeviceMemoryProperties& getMemProperties() const
	{
		assert(m_initialized);
		return m_memProperties;
	}

private:

	friend class VKInstance;

	void initialize(not_null<VKInstance*> instance, vk::PhysicalDevice physDevice);
	void cleanup();

private:

	bool m_initialized              = false;
	VKInstance* m_instance          = NULL;
	vk::PhysicalDevice m_physDevice = NULL;
	vk::PhysicalDeviceMemoryProperties m_memProperties;
	eastl::vector<vk::QueueFamilyProperties> m_queueFamilyProperties;
	VKDevice m_devices[uint(VKDevice::EDeviceType::NUM_TYPES)];

	VKSwapchain m_swapchain;
	uint m_graphicsQueueNodeIndex = UINT32_MAX;
};