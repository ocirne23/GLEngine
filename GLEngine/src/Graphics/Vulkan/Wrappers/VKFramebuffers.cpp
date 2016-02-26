#include "Graphics/Vulkan/Wrappers/VKFramebuffers.h"

VKFramebuffers::~VKFramebuffers()
{
	if (m_initialized)
		cleanup();
}

void VKFramebuffers::initialize(VKDevice & device, VKSwapchain& swapchain, uint a_width, uint a_height)
{
	if (m_initialized)
		cleanup();
	/*
	vk::ImageView attachments[2];
	attachments[1] = a_depthStencil.view;

	vk::FramebufferCreateInfo framebufferCreateInfo = vk::FramebufferCreateInfo()
		.renderPass(a_renderPass)
		.attachmentCount(2)
		.pAttachments(attachments)
		.width(a_width)
		.height(a_height)
		.layers(1);

	eastl::vector<vk::Framebuffer> framebuffers;
	framebuffers.resize(a_swapChain.getNumImages());
	for (uint i = 0; i < a_swapChain.getNumImages(); ++i)
	{
	attachments[0] = a_swapChain.getBuffer(i).view;
	VKVerifier result = vk::createFramebuffer(a_device, &framebufferCreateInfo, NULL, &framebuffers[i]);
	}*/


	m_initialized = false;
}

void VKFramebuffers::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}