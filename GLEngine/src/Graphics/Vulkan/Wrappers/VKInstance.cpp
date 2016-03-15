#include "Graphics/Vulkan/Wrappers/VKInstance.h"

#include "Graphics/Vulkan/Utils/VKDebug.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKInstance::~VKInstance()
{
	if (m_initialized)
		cleanup();
}

void VKInstance::initialize()
{
	if (m_initialized)
		cleanup();

	eastl::vector<const char*> enabledExtensions = { 
		VK_KHR_SURFACE_EXTENSION_NAME, 
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME 
	};
	if (VKDebug::VALIDATION_ENABLED)
		enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.apiVersion(VK_API_VERSION)
		.engineVersion(0)
		.pEngineName("GLEngine")
		.applicationVersion(0)
		.pApplicationName("GLApp");

	vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
		.pApplicationInfo(&appInfo);

	instanceCreateInfo.enabledExtensionCount(uint(enabledExtensions.size()));
	instanceCreateInfo.ppEnabledExtensionNames(enabledExtensions.data());

	if (VKDebug::VALIDATION_ENABLED)
	{
		instanceCreateInfo.enabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT); // todo : change validation layer names!
		instanceCreateInfo.ppEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	VKVerifier result = vk::createInstance(&instanceCreateInfo, NULL, &m_instance);

	eastl::vector<vk::PhysicalDevice> physDevices;
	result = vk::enumeratePhysicalDevices(m_instance, physDevices);
	
	m_physicalDevices.resize(physDevices.size());
	for (uint i = 0; i < m_physicalDevices.size(); ++i)
		m_physicalDevices[i].initialize(this, physDevices[i]);

	m_initialized = true;
}

VKPhysicalDevice& VKInstance::getPhysicalDevice(uint a_index)
{
	assert(m_initialized);
	if (m_physicalDevices.size() > a_index)
		return m_physicalDevices[a_index];
	else
	{
		assert(false);
		return m_physicalDevices[0];
	}
}

void VKInstance::cleanup()
{
	assert(m_initialized);

	assert(false); //TODO

	m_initialized = false;
}