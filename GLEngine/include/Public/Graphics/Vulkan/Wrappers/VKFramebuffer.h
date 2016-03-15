#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "gsl/gsl.h"

class VKDevice;
class VKSwapchain;
class VKRenderPass;

class VKFramebuffer
{
public:

	VKFramebuffer() {}
	VKFramebuffer(const VKFramebuffer& copy) { assert(!m_initialized); }
	~VKFramebuffer();
	void initialize(VKDevice& device, VKRenderPass& renderPass, span<const vk::ImageView> attachments, uint width, uint height);
	void cleanup();

	bool isInitialized() const { return m_initialized; }

private:
	
	bool m_initialized = false;
	vk::Framebuffer m_framebuffer;
};