#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKCommandBuffer;
class VKInstance;
class VKPhysicalDevice;

class VKSwapchain
{
public:

	~VKSwapchain();
	void initialize(VKInstance& instance, VKPhysicalDevice& physDevice);
	void setup(VKCommandBuffer& setupCommandBuffer, uint width, uint height);
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

	bool m_initialized             = false;
	bool m_setup                   = false;
	VKInstance* m_instance         = NULL;
	VKPhysicalDevice* m_physDevice = NULL;
	vk::SwapchainKHR m_swapchain   = NULL;
	vk::SurfaceKHR m_surface       = NULL;
	uint m_graphicsQueueNodeIndex  = UINT32_MAX;
	uint m_presentQueueNodeIndex   = UINT32_MAX;
	vk::Format m_colorFormat       = vk::Format::eUndefined;
	vk::ColorSpaceKHR m_colorSpace;

	eastl::vector<vk::Image> m_images;
	eastl::vector<vk::ImageView> m_views;
};