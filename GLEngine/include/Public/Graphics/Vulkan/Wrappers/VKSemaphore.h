#pragma once

#include "Core.h"
#include "Graphics\Vulkan\vk_cpp.h"

class VKSemaphore
{
public:

	VKSemaphore() {}
	VKSemaphore(const VKSemaphore& copy) { assert(!m_initialized); }
	~VKSemaphore();

	void initialize();
	void cleanup();

private:

	bool m_initialized = false;
};