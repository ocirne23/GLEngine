#include "Graphics/Vulkan/Wrappers/VKBuffer.h"

#include "Graphics/Vulkan/VKContext.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"

VKBuffer::~VKBuffer()
{
	if (m_initialized)
		cleanup();
}

void VKBuffer::initialize(uint64 a_size, vk::BufferUsageFlags a_useage, vk::MemoryPropertyFlags a_memoryProperty, EUseStagingBuffer a_useStagingBuffer)
{
	if (m_initialized)
		cleanup();

	m_size = a_size;
	m_useStagingBuffer = (a_useStagingBuffer == EUseStagingBuffer::YES);

	if (m_useStagingBuffer)
	{
		// Using a staging buffer usually implies you want device local storage?
		assert(a_memoryProperty & vk::MemoryPropertyFlagBits::eDeviceLocal);
	}

	if (m_useStagingBuffer)
	{
		m_stagingBuffer = new VKBuffer();
		m_stagingBuffer->initialize(a_size, a_useage | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible, EUseStagingBuffer::NO);
	}

	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.setSize(a_size)
		.setUsage(a_useage | (m_useStagingBuffer ? vk::BufferUsageFlagBits::eTransferDst : vk::BufferUsageFlags()));
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
	SAFE_DELETE(m_stagingBuffer);
	m_initialized = false;
}

void* VKBuffer::mapMemory()
{
	assert(!m_mapped && m_initialized);
	assert(!m_useStagingBuffer || m_stagingBuffer);

	m_mapped = true;
	if (m_useStagingBuffer)
		return m_stagingBuffer->mapMemory();
	else
		return VKContext::device.mapMemory(m_memory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags());
}

void VKBuffer::unmapMemory()
{
	assert(m_mapped && m_initialized);
	assert(!m_useStagingBuffer || m_stagingBuffer);

	m_mapped = false;
	if (m_useStagingBuffer)
		m_stagingBuffer->unmapMemory();
	else
		VKContext::device.unmapMemory(m_memory);
}

void VKBuffer::uploadWithStaging(vk::CommandBuffer a_commandBuffer)
{
	assert(m_initialized && m_useStagingBuffer && m_stagingBuffer);
	a_commandBuffer.copyBuffer(*m_stagingBuffer, m_buffer, vk::BufferCopy().setSize(m_size));
}

void VKBuffer::cleanupStagingBuffer()
{
	assert(m_initialized && m_useStagingBuffer && m_stagingBuffer);
	SAFE_DELETE(m_stagingBuffer);
}
