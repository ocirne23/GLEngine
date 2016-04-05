#pragma once

#include "Core.h"
#include "Graphics\Vulkan\vk_cpp.h"

class VKDevice;

class VKBuffer
{
public:

	VKBuffer() {}
	VKBuffer(const VKBuffer& copy) { assert(!m_initialized); }
	~VKBuffer();
	void initialize(VKDevice& device, vk::BufferUsageFlags usage, vk::MemoryPropertyFlagBits a_propertyFlags, uint64 sizeBytes);
	void cleanup();

	void* mapBuffer();
	void unmapBuffer();
	void bindMemory();

	vk::Buffer getVKBuffer() { return m_buffer; }
	vk::DeviceMemory getVKDeviceMemory() { return m_memory; }
	uint64 getSizeBytes() const { return m_sizeBytes; }

private:

	bool m_initialized        = false;
	bool m_mapped             = false;
	vk::Device m_device       = NULL;
	vk::DeviceMemory m_memory = NULL;
	vk::Buffer m_buffer       = NULL;
	uint64 m_sizeBytes        = 0;
};