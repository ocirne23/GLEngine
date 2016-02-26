#include "Graphics/Vulkan/Wrappers/VKDepthStencil.h"

VKDepthStencil::~VKDepthStencil()
{
	if (m_initialized)
		cleanup();
}

void VKDepthStencil::initialize()
{
	if (m_initialized)
		cleanup();

	m_initialized = true;
}

void VKDepthStencil::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}
