#include "Graphics/Vulkan/Wrappers/VKSemaphore.h"

VKSemaphore::~VKSemaphore()
{
	if (m_initialized)
		cleanup();
}

void VKSemaphore::initialize()
{
	if (m_initialized)
		cleanup();

	m_initialized = true;
}

void VKSemaphore::cleanup()
{
	m_initialized = false;
}
