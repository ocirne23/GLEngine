#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKCommandPool
{
public:

	VKCommandPool() {}
	VKCommandPool(const VKCommandPool& copy) { assert(!m_initialized); }
	~VKCommandPool();

	bool isInitialized() const { return m_initialized; }
	vk::CommandPool getVKCommandPool() { assert(m_initialized); return m_pool; }

private:
	friend class VKDevice;

	void initialize(vk::Device device, uint queueNodeIndex);
	void cleanup();

private:

	bool m_initialized = false;
	vk::CommandPool m_pool;
};