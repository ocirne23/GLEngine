#include "Graphics/Vulkan/Wrappers/VKBuffer.h"

#include "Graphics/Vulkan/VKContext.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"

VKBuffer::~VKBuffer()
{
	if (m_initialized)
		cleanup();
}

void VKBuffer::initialize(uint64 a_size, vk::BufferUsageFlags a_useage, vk::MemoryPropertyFlags a_memoryProperty)
{
	if (m_initialized)
		cleanup();

	m_size = a_size;

	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.setSize(a_size)
		.setUsage(a_useage); // Buffer is used as the copy source
	m_buffer = VKContext::device.createBuffer(bufferCreateInfo, NULL);

	vk::MemoryRequirements memReqs = VKContext::device.getBufferMemoryRequirements(m_buffer);
	vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(memReqs.size)
		.setMemoryTypeIndex(VKUtils::getMemoryType(VKContext::physDeviceMemoryProperties, memReqs.memoryTypeBits, a_memoryProperty));
	m_memory = VKContext::device.allocateMemory(allocInfo, NULL);
	VKContext::device.bindBufferMemory(m_buffer, m_memory, 0);

	m_initialized = true;
}

void VKBuffer::cleanup()
{
	assert(m_initialized);
	VKContext::device.destroyBuffer(m_buffer);
	VKContext::device.freeMemory(m_memory);
	m_initialized = false;
}

void* VKBuffer::mapMemory()
{
	assert(!m_mapped && m_initialized);
	m_mapped = true;
	return VKContext::device.mapMemory(m_memory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags());
}

void VKBuffer::unmapMemory()
{
	assert(m_mapped && m_initialized);
	VKContext::device.unmapMemory(m_memory);
	m_mapped = false;
}

void VKBuffer::uploadWithStaging(vk::CommandBuffer commandBuffer)
{
	VKBuffer stagingBuffer;

}
