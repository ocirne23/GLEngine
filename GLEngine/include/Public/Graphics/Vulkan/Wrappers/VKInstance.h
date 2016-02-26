#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "EASTL/vector.h"

class VKInstance
{
public:

	~VKInstance();

	void initialize();
	
	uint getNumPhysicalDevices() const 
	{ 
		assert(m_initialized); 
		return uint(m_physicalDevices.size()); 
	}
	
	vk::Instance getVKInstance() 
	{ 
		assert(m_initialized);  
		return m_instance; 
	}

	VKPhysicalDevice getPhysicalDevice(uint index = 0);

private:
	
	void cleanup();

private:
	
	bool m_initialized = false;

	vk::Instance m_instance = NULL;
	eastl::vector<VKPhysicalDevice> m_physicalDevices;
};