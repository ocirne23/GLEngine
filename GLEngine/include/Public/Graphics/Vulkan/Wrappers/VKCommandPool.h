#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"


class VKCommandPool
{
public:

	bool isInitialized() const { return m_initialized; }
	vk::CommandPool getVKCommandPool() { return m_pool; }

private:
	friend class VKDevice;

	void initialize(vk::Device device, uint queueNodeIndex);
	void cleanup();

private:

	bool m_initialized = false;
	vk::CommandPool m_pool;
};