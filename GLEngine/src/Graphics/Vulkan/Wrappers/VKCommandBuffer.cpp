#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

void VKCommandBuffer::initialize(VKDevice& a_device)
{
	if (m_initialized)
		cleanup();

	eastl::vector<vk::CommandBuffer> buffers;
	buffers.resize(1);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.commandPool(a_device.getCommandPool().getVKCommandPool())
		.level(vk::CommandBufferLevel::ePrimary)
		.commandBufferCount(1);

	vk::allocateCommandBuffers(a_device.getVKDevice(), &commandBufferAllocateInfo, &m_commandBuffer);

	m_initialized = true;
}

void VKCommandBuffer::cleanup()
{
	assert(m_initialized);

	assert(false); //TODO

	m_initialized = false;
}

void VKCommandBuffer::begin()
{
	assert(!m_begun);
	vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();
	vk::beginCommandBuffer(m_commandBuffer, commandBufferBeginInfo);
	m_begun = true;
}

void VKCommandBuffer::end()
{
	assert(m_begun);
	vk::endCommandBuffer(m_commandBuffer);
	m_begun = false;
}
