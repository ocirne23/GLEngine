#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKBuffer
{
public:

	~VKBuffer();

	void initialize(uint64 size, vk::BufferUsageFlags useage, vk::MemoryPropertyFlags memoryProperty);
	void cleanup();

	void* mapMemory();
	void unmapMemory();

	void uploadWithStaging(vk::CommandBuffer commandBuffer);

	operator vk::Buffer() { return m_buffer; };

private:

	vk::Buffer m_buffer = NULL;
	vk::DeviceMemory m_memory = NULL;
	uint64 m_size = 0;
	bool m_initialized = false;
	bool m_mapped = false;
};