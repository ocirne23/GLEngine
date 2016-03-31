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

	m_commandBuffer = a_device.getVKDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];

	m_initialized = true;
}

void VKCommandBuffer::cleanup()
{
	assert(m_initialized);
	assert(!m_begun);

	m_device->getVKQueue().waitIdle(); // Maybe seperate?
	m_device->getVKDevice().freeCommandBuffers(m_device->getCommandPool().getVKCommandPool(), 1, &m_commandBuffer);
	m_commandBuffer = VK_NULL_HANDLE;

	m_initialized = false;
}

void VKCommandBuffer::begin()
{
	assert(m_initialized);
	assert(!m_begun);
	vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();
	m_commandBuffer.begin(commandBufferBeginInfo);
	m_begun = true;
}

void VKCommandBuffer::end()
{
	assert(m_initialized);
	assert(m_begun);
	m_commandBuffer.end();
	m_begun = false;
}

void VKCommandBuffer::submit()
{
	assert(m_initialized);
	assert(!m_begun);

	vk::SubmitInfo submitInfo = vk::SubmitInfo()
		.commandBufferCount(1)
		.pCommandBuffers(&m_commandBuffer);

	VKVerifier result = m_device->getVKQueue().submit(1, &submitInfo, VK_NULL_HANDLE);
}

void VKCommandBuffer::waitIdle()
{
	assert(m_initialized);
	m_device->getVKQueue().waitIdle();
}