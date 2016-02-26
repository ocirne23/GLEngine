#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "EASTL/vector.h"

class VKSwapchainTMP
{
public:

	typedef struct _SwapChainBuffers {
		vk::Image image;
		vk::ImageView view;
	} SwapChainBuffer;

public:

	void initialize(vk::Instance instance, vk::PhysicalDevice physDevice);
	void setup(vk::Device device, vk::CommandBuffer commandBuffer, uint& width, uint& height);

	uint getGraphicsQueueIndex() const { return m_graphicsQueueIndex; }
	uint getNumImages() const          { return uint(m_images.size()); }

	SwapChainBuffer& getBuffer(uint index) { return m_buffers[0]; }
	vk::Image getImage(uint index) { return m_images[0]; }

private:

	vk::Instance m_instance = VK_NULL_HANDLE;;
	vk::Device m_device = VK_NULL_HANDLE;;
	vk::PhysicalDevice m_physDevice = VK_NULL_HANDLE;;
	vk::SurfaceKHR m_surface = VK_NULL_HANDLE;;
	vk::Format m_colorFormat;
	vk::ColorSpaceKHR m_colorSpace;
	vk::SwapchainKHR m_swapchain = VK_NULL_HANDLE;
	eastl::vector<vk::Image> m_images;
	eastl::vector<SwapChainBuffer> m_buffers;
	uint m_graphicsQueueIndex = 0;
};