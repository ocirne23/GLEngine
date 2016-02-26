#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;

class VKCommandBuffer
{
public:

	~VKCommandBuffer();

	void initialize(VKDevice& device);
	void cleanup();

	void begin();
	void end();

	vk::CommandBuffer getVKCommandBuffer()
	{
		assert(m_initialized);
		return m_commandBuffer;
	}

private:

	bool m_initialized = false;
	bool m_begun = false;
	vk::CommandBuffer m_commandBuffer;
};