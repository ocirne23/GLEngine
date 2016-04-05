#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"

class VKCommandBuffer;
class VKInstance;
class VKPhysicalDevice;

class VKSwapchain
{
public:

	VKSwapchain() {}
	VKSwapchain(const VKSwapchain& copy) { assert(!m_initialized); }
	~VKSwapchain();

	void initialize(VKPhysicalDevice& physDevice, VKCommandBuffer& setupCmdBuffer, uint width, uint height);
	bool isInitialized() const { return m_initialized; }
	void cleanup();
	void acquireNextImage(vk::Semaphore presentCompleteSemaphore, uint& currentBuffer);

	vk::SwapchainKHR getVKSwapchain() 
	{ 
		assert(m_initialized); 
		return m_swapchain; 
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

	uint getNumImages() const { return uint(m_images.size()); }
	uint getNumViews() const { return uint(m_views.size()); }

	const eastl::vector<vk::Image>& getImages() const { return m_images; }
	const eastl::vector<vk::ImageView>& getViews() const { return m_views; }
	vk::ImageView getView(uint a_index) 
	{ 
		assert(m_initialized && m_views.size() > a_index); 
		return m_views[a_index];
	}
	vk::Image getImage(uint a_index)
	{
		assert(m_initialized && m_images.size() > a_index);
		return m_images[a_index];
	}

private:

	bool m_initialized              = false;
	vk::Instance m_instance         = NULL;
	vk::PhysicalDevice m_physDevice = NULL;
	vk::Device m_device             = NULL;
	vk::SwapchainKHR m_swapchain    = NULL;
	vk::SurfaceKHR m_surface        = NULL;
	vk::Format m_colorFormat        = vk::Format::eUndefined;
	vk::ColorSpaceKHR m_colorSpace;

	eastl::vector<vk::Image> m_images;
	eastl::vector<vk::ImageView> m_views;
};