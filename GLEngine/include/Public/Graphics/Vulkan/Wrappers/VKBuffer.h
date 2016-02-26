#pragma once

#include "Core.h"

#include "Graphics\Vulkan\vk_cpp.h"

class VKBuffer
{
public:

	~VKBuffer();

	void initialize();

	void cleanup();

private:

	bool m_initialized        = false;
	vk::Device m_device       = NULL;
	vk::DeviceMemory m_memory = NULL;
	uint64 m_size             = 0;
};