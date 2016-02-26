#pragma once

#include "Core.h"

#include "Graphics\Vulkan\vk_cpp.h"

class VKBuffer
{
public:

	void initialize();

private:

	vk::Device m_device       = NULL;
	vk::DeviceMemory m_memory = NULL;
	uint64 m_size             = 0;
};