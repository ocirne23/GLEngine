#include "Graphics/Vulkan/Wrappers/VKRenderPass.h"

VKRenderPass::~VKRenderPass()
{
	if (m_initialized)
		cleanup();
}

void VKRenderPass::initialize()
{
	if (m_initialized)
		cleanup();

	m_initialized = true;
}

void VKRenderPass::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}
