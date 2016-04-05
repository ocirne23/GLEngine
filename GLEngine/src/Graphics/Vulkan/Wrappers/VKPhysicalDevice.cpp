#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Wrappers/VKInstance.h"
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

	m_memProperties = m_physDevice.getMemoryProperties();
	m_queueFamilyProperties = m_physDevice.getQueueFamilyProperties();

	m_initialized = true;
}

void VKPhysicalDevice::cleanup()
{
	assert(m_initialized);

	assert(false); // TODO

	m_initialized = false;
}

uint VKPhysicalDevice::getQueueFamilyIndex(VKDevice::EDeviceType a_deviceType)
{
	for (uint i = 0; i < m_queueFamilyProperties.size(); ++i)
	{
		vk::QueueFlags flags = m_queueFamilyProperties[i].queueFlags();
		if (a_deviceType == VKDevice::EDeviceType::Graphics)
			if (flags & vk::QueueFlagBits::eGraphics)
				return i;
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

	assert(false);
	return UINT32_MAX;
}

vk::Format VKPhysicalDevice::getDepthFormat()
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	eastl::vector<vk::Format> depthFormats = {
		vk::Format::eD24UnormS8Uint, 
		vk::Format::eD16UnormS8Uint, 
		vk::Format::eD16Unorm 
	};
	for (auto& format : depthFormats)
	{
		vk::FormatProperties formatProperties;
		m_physDevice.getFormatProperties(format, &formatProperties);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProperties.optimalTilingFeatures() & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
			return format;
	}
	assert(false);
	return vk::Format::eUndefined;
}

vk::Instance VKPhysicalDevice::getVKInstance()
{
	assert(m_initialized);
	return m_instance->getVKInstance();
}

VKDevice& VKPhysicalDevice::getDevice(VKDevice::EDeviceType a_deviceType)
{
	assert(m_initialized);
	VKDevice& device = m_devices[uint(a_deviceType)];
	if (!device.isInitialized())
		device.initialize(*this, a_deviceType);
	return device;
}