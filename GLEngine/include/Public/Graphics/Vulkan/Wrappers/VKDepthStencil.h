#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;
class VKCommandBuffer;

class VKDepthStencil
{
public:

	VKDepthStencil() {}
	VKDepthStencil(const VKDepthStencil& copy) { assert(!m_initialized); }
	~VKDepthStencil();
	void initialize(VKDevice& a_device, VKCommandBuffer& a_setupCommandBuffer, uint a_width, uint a_height);
	void cleanup();

	bool isInitialized() const { return m_initialized; }

private:

	bool m_initialized = false;

	vk::Image m_image;
	vk::ImageView m_view;
	vk::DeviceMemory m_mem;

};