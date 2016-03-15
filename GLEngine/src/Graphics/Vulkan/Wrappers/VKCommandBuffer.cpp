#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKCommandBuffer::~VKCommandBuffer()
{
	if (m_initialized)
		cleanup();
}

void VKCommandBuffer::initialize(VKDevice& a_device)
{
	assert(!m_begun);

	if (m_initialized)
		cleanup();

	m_device = &a_device;

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
	assert(!m_begun);

	VKVerifier result = vk::queueWaitIdle(m_device->getVKQueue()); // Maybe seperate?

	vk::freeCommandBuffers(m_device->getVKDevice(), m_device->getCommandPool().getVKCommandPool(), 1, &m_commandBuffer);
	m_commandBuffer = VK_NULL_HANDLE;

	m_initialized = false;
}

void VKCommandBuffer::begin()
{
	assert(m_initialized);
	assert(!m_begun);
	vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();
	vk::beginCommandBuffer(m_commandBuffer, commandBufferBeginInfo);
	m_begun = true;
}

void VKCommandBuffer::end()
{
	assert(m_initialized);
	assert(m_begun);
	vk::endCommandBuffer(m_commandBuffer);
	m_begun = false;
}

void VKCommandBuffer::submit()
{
	assert(m_initialized);
	assert(!m_begun);

	vk::SubmitInfo submitInfo = vk::SubmitInfo()
		.commandBufferCount(1)
		.pCommandBuffers(&m_commandBuffer);

	VKVerifier result = vk::queueSubmit(m_device->getVKQueue(), 1, &submitInfo, VK_NULL_HANDLE);
}
