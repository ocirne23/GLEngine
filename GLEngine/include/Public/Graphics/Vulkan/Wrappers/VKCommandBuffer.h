#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;

class VKCommandBuffer
{
public:

	VKCommandBuffer() {}
	VKCommandBuffer(const VKCommandBuffer& copy) { assert(!m_initialized); }
	~VKCommandBuffer();

	void initialize(VKDevice& device);
	void cleanup();

	void begin();
	void end();
	void submit();

	vk::CommandBuffer getVKCommandBuffer()
	{
		assert(m_initialized);
		return m_commandBuffer;
	}

	bool isInitialized() const { return m_initialized; }
	bool isBegun() const { return m_begun; }

private:

	bool m_initialized = false;
	bool m_begun = false;
	vk::CommandBuffer m_commandBuffer;
	VKDevice* m_device;
};