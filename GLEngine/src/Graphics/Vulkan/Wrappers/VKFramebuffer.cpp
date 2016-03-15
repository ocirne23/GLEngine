#include "Graphics/Vulkan/Wrappers/VKFramebuffer.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"
#include "Graphics/Vulkan/Wrappers/VKRenderPass.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKFramebuffer::~VKFramebuffer()
{
	if (m_initialized)
		cleanup();
}

void VKFramebuffer::initialize(VKDevice& a_device, VKRenderPass& a_renderPass, span<const vk::ImageView> a_attachments, uint a_width, uint a_height)
{
	if (m_initialized)
		cleanup();

	vk::FramebufferCreateInfo framebufferCreateInfo = vk::FramebufferCreateInfo()
		.renderPass(a_renderPass.getVKRenderPass())
		.attachmentCount(uint(a_attachments.size()))
		.pAttachments(&a_attachments[0])
		.width(a_width)
		.height(a_height)
		.layers(1);

	VKVerifier result = vk::createFramebuffer(a_device.getVKDevice(), &framebufferCreateInfo, NULL, &m_framebuffer);

	m_initialized = false;
}

void VKFramebuffer::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}