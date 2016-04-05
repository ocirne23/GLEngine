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

	m_device = a_device.getVKDevice();
	m_sizeBytes = a_sizeBytes;

	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.size(a_sizeBytes)
		.usage(a_usage);
	m_buffer = m_device.createBuffer(bufferCreateInfo, NULL);

	vk::MemoryRequirements memReqs = m_device.getBufferMemoryRequirements(m_buffer);
	uint memoryTypeIndex = VKUtils::getMemoryType(a_device.getPhysDevice()->getMemProperties(), memReqs.memoryTypeBits(), a_propertyFlags);
	vk::MemoryAllocateInfo memAlloc = vk::MemoryAllocateInfo()
		.allocationSize(memReqs.size())
		.memoryTypeIndex(memoryTypeIndex);
	m_memory = m_device.allocateMemory(memAlloc, NULL);

	m_initialized = true;
}

void VKBuffer::cleanup()
{
	assert(m_initialized);
	assert(!m_mapped);

	m_device.destroyBuffer(m_buffer, NULL);
	m_device.freeMemory(m_memory, NULL);

	m_device = NULL;
	m_initialized = false;
}

void* VKBuffer::mapBuffer()
{
	assert(m_initialized);
	assert(!m_mapped);
	void* data = m_device.mapMemory(m_memory, 0, m_sizeBytes, vk::MemoryMapFlags());
	m_mapped = true;
	return data;
}

void VKBuffer::unmapBuffer()
{
	assert(m_initialized);
	assert(m_mapped);
	m_device.unmapMemory(m_memory);
	m_mapped = false;
}

void VKBuffer::bindMemory()
{
	assert(m_initialized);
	m_device.bindBufferMemory(m_buffer, m_memory, 0);
}