#include "Graphics/Vulkan/Wrappers/VKBuffer.h"

VKBuffer::~VKBuffer()
{
	if (m_initialized)
		cleanup();
}

void VKBuffer::initialize()
{
	if (m_initialized)
		cleanup();
}

void VKBuffer::cleanup()
{
	assert(m_initialized);
	assert(false); // TODO
	m_initialized = false;
}
