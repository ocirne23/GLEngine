#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "EASTL/vector.h"

class Swapchain
{
public:

	typedef struct _SwapChainBuffers {
		vk::Image image;
		vk::ImageView view;
	} SwapChainBuffer;

public:

	void initialize(vk::Instance instance, vk::PhysicalDevice physDevice, vk::Device device);
	void setup(vk::CommandBuffer commandBuffer, uint& width, uint& height);

	
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