#include "Graphics/Vulkan/VKApiTest.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

void VKApiTest::test()
{
	uint width = GLEngine::graphics->getWindowWidth();
	uint height = GLEngine::graphics->getWindowHeight();

	m_instance.initialize();
	m_physDevice = m_instance.getPhysicalDevice();
	m_device = &m_physDevice.getDevice(VKDevice::EDeviceType::Graphics);
	m_setupCommandBuffer.initialize(*m_device);

	m_setupCommandBuffer.begin();
	m_physDevice.getSwapchain().setup(m_setupCommandBuffer, width, height);

	m_drawCommandBuffer.initialize(*m_device);
	m_postPresentCommandBuffer.initialize(*m_device);
}