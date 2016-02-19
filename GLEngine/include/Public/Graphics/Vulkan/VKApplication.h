#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKApplication
{
public:

public:

	void initialize();

private:

	vk::Result createInstance(bool enableValidation);
	vk::Result createDevice(vk::DeviceQueueCreateInfo requestedQueues, bool enableValidation);

private:

	vk::Instance m_instance = NULL;
	vk::PhysicalDevice m_physDevice = NULL;
	vk::PhysicalDeviceMemoryProperties physDeviceMemoryProperties;
	vk::Device m_device = NULL;
	vk::Queue m_queue = NULL;
	vk::Format m_colorFormat = vk::Format::eB8G8R8A8Unorm;
	vk::Format m_depthFormat;
	vk::CommandPool m_commandPool = NULL;

	vk::CommandBuffer m_setupCommandBuffer = NULL;
	vk::CommandBuffer m_postPresentCommandBuffer = NULL;
	eastl::vector<vk::CommandBuffer> m_drawCommandBuffers;
	vk::RenderPass m_renderPass = NULL;
	eastl::vector<vk::Framebuffer> m_framebuffers;
	uint m_activeFBIdx = 0;
	vk::DescriptorPool m_descriptorPool = NULL;
	eastl::vector<vk::ShaderModule> m_shaderModules;
	vk::PipelineCache m_pipelineCache = NULL;
	vk::SwapchainKHR m_swapchain = NULL;

	bool m_enableValidation = false;
	bool m_prepared = false;
};