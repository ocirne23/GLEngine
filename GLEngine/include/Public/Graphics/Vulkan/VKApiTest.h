#pragma once

#include "Core.h"
#include "Graphics/Vulkan/Wrappers/VKInstance.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"

class VKApiTest
{
public:

	void test();

private:

	VKInstance m_instance;
	VKPhysicalDevice m_physDevice;
	VKDevice* m_device;

	VKCommandBuffer m_setupCommandBuffer;
	VKCommandBuffer m_drawCommandBuffer;
	VKCommandBuffer m_postPresentCommandBuffer;
};