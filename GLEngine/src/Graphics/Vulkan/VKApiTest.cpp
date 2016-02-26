#include "Graphics/Vulkan/VKApiTest.h"

void VKApiTest::test()
{
	m_instance.initialize();
	m_physDevice = m_instance.getPhysicalDevice();
	m_device = &m_physDevice.getDevice(VKDevice::EDeviceType::Graphics);

	m_setupCommandBuffer.initialize(*m_device);
}