#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKDevice.h"

#include "gsl/gsl.h"
#include "EASTL/vector.h"

class VKInstance;

class VKPhysicalDevice
{
public:

	VKPhysicalDevice() {}
	VKPhysicalDevice(const VKPhysicalDevice& copy) { assert(!m_initialized); }
	~VKPhysicalDevice();

	bool isInitialized() const { return m_initialized; }

	VKDevice& getDevice(VKDevice::EDeviceType deviceType = VKDevice::EDeviceType::Graphics);
	uint getQueueFamilyIndex(VKDevice::EDeviceType deviceType);

	vk::Format getDepthFormat();

	vk::Instance getVKInstance();

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
};