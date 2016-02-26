#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKPhysicalDevice::~VKPhysicalDevice()
{
	if (m_initialized)
		cleanup();
}

void VKPhysicalDevice::initialize(not_null<VKInstance*> a_instance, vk::PhysicalDevice a_physDevice)
{
	if (m_initialized)
		cleanup();

	m_instance = a_instance;
	m_physDevice = a_physDevice;

	vk::getPhysicalDeviceMemoryProperties(m_physDevice, m_memProperties);
	m_queueFamilyProperties = vk::getPhysicalDeviceQueueFamilyProperties(m_physDevice);

	m_initialized = true;
}

void VKPhysicalDevice::cleanup()
{
	assert(m_initialized);

	assert(false); // TODO

	m_initialized = false;
}

uint VKPhysicalDevice::getQueueNodeIndex(VKDevice::EDeviceType a_deviceType)
{
	if (a_deviceType == VKDevice::EDeviceType::Graphics)
	{
		return getSwapchain().getGraphicsQueueNodeIndex();
	}
	else
	{
		for (uint i = 0; i < m_queueFamilyProperties.size(); ++i)
		{
			vk::QueueFlags flags = m_queueFamilyProperties[i].queueFlags();
			if (a_deviceType == VKDevice::EDeviceType::Compute)
				if (flags & vk::QueueFlagBits::eCompute)
					return i;
			if (a_deviceType == VKDevice::EDeviceType::Transfer)
				if (flags & vk::QueueFlagBits::eTransfer)
					return i;
			if (a_deviceType == VKDevice::EDeviceType::SparseBinding)
				if (flags & vk::QueueFlagBits::eSparseBinding)
					return i;
		}
	}

	assert(false);
	return UINT32_MAX;
}

VKDevice& VKPhysicalDevice::getDevice(VKDevice::EDeviceType a_deviceType)
{
	assert(m_initialized);
	VKDevice& device = m_devices[uint(a_deviceType)];
	if (!device.isInitialized())
		device.initialize(*this, a_deviceType, getQueueNodeIndex(a_deviceType));
	return device;
}

VKSwapchain& VKPhysicalDevice::getSwapchain()
{
	assert(m_initialized);
	if (!m_swapchain.isInitialized())
		m_swapchain.initialize(*m_instance, *this);
	return m_swapchain;
}