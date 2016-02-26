#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;
class VKSwapchain;

class VKFramebuffers
{
public:

	~VKFramebuffers();
	void initialize(VKDevice& device, VKSwapchain& swapchain, uint width, uint height);
	void cleanup();

	bool isInitialized() const { return m_initialized; }

private:
	
	bool m_initialized = false;
	eastl::vector<vk::Framebuffer> m_framebuffers;
};