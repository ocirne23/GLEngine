#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKInstance;
class VKPhysicalDevice;

class VKSwapchain
{
public:

	void initialize(VKInstance& instance, VKPhysicalDevice& physDevice);
	void cleanup();

	bool isInitialized() const { return m_initialized; }

	uint getGraphicsQueueNodeIndex() const 
	{ 
		assert(m_initialized); 
		return m_graphicsQueueNodeIndex; 
	}

	uint getPresentQueueNodeIndex() const
	{
		assert(m_initialized);
		return m_presentQueueNodeIndex;
	}

	vk::Format getColorFormat() const
	{
		assert(m_initialized);
		return m_colorFormat;
	}

	vk::ColorSpaceKHR getColorSpace() const
	{
		assert(m_initialized);
		return m_colorSpace;
	}

private:

	bool m_initialized = false;

	vk::SurfaceKHR m_surface;
	uint m_graphicsQueueNodeIndex = UINT32_MAX;
	uint m_presentQueueNodeIndex  = UINT32_MAX;
	vk::Format m_colorFormat = vk::Format::eUndefined;
	vk::ColorSpaceKHR m_colorSpace;
};