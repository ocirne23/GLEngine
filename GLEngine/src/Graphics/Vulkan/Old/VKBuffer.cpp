#include "Graphics/Vulkan/Wrappers/VKBuffer.h"

#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"

VKBuffer::~VKBuffer()
{
	if (m_initialized)
		cleanup();
}

void VKBuffer::initialize(VKDevice& a_device, vk::BufferUsageFlags a_usage, vk::MemoryPropertyFlagBits a_propertyFlags, uint64 a_sizeBytes)
{
	assert(!m_mapped);

	if (m_initialized)
		cleanup();

	m_device = &a_device;
	m_sizeBytes = a_sizeBytes;

	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.size(a_sizeBytes)
		.usage(a_usage);
	VKVerifier result = vk::createBuffer(a_device.getVKDevice(), &bufferCreateInfo, NULL, &m_buffer);

	vk::MemoryRequirements memReqs;
	vk::getBufferMemoryRequirements(a_device.getVKDevice(), m_buffer, memReqs);	

	uint memoryTypeIndex = VKUtils::getMemoryType(a_device.getPhysDevice()->getMemProperties(), memReqs.memoryTypeBits(), a_propertyFlags);
	vk::MemoryAllocateInfo memAlloc = vk::MemoryAllocateInfo()
		.allocationSize(memReqs.size())
		.memoryTypeIndex(memoryTypeIndex);
	result = vk::allocateMemory(a_device.getVKDevice(), &memAlloc, NULL, &m_memory);

	m_initialized = true;
}

void VKBuffer::cleanup()
{
	assert(m_initialized);
	assert(!m_mapped);

	vk::destroyBuffer(m_device->getVKDevice(), m_buffer, NULL);
	vk::freeMemory(m_device->getVKDevice(), m_memory, NULL);

	m_device = NULL;
	m_initialized = false;
}

void* VKBuffer::mapBuffer()
{
	assert(m_initialized);
	assert(!m_mapped);
	void* data = NULL;
	VKVerifier result = vk::mapMemory(m_device->getVKDevice(), m_memory, 0, m_sizeBytes, 0, &data);
	m_mapped = true;
	return data;
}

void VKBuffer::unmapBuffer()
{
	assert(m_initialized);
	assert(m_mapped);
	vk::unmapMemory(m_device->getVKDevice(), m_memory);
	VKVerifier result = vk::bindBufferMemory(m_device->getVKDevice(), m_buffer, m_memory, 0);
	m_mapped = false;
}

void VKBuffer::bindMemory()
{
	assert(m_initialized);
	VKVerifier result = vk::bindBufferMemory(m_device->getVKDevice(), m_buffer, m_memory, 0);
}
