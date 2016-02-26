#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKCommandPool.h"

class VKPhysicalDevice;

class VKDevice
{
public:

	enum class EDeviceType { Graphics, Compute, Transfer, SparseBinding, NUM_TYPES, Uninitialized };

public:

	~VKDevice();

	bool isInitialized() const { return m_initialized; }

	vk::Device getVKDevice() 
	{ 
		assert(m_initialized); 
		return m_device; 
	}

	VKPhysicalDevice* getPhysDevice() 
	{ 
		assert(m_initialized); 
		return m_physDevice; 
	}

	VKCommandPool getCommandPool();


private:

	friend class VKPhysicalDevice;

	void initialize(VKPhysicalDevice& physDevice, EDeviceType type, uint queueFamilyIndex);
	void cleanup();

private:

	bool m_initialized = false;
	VKPhysicalDevice* m_physDevice;
	EDeviceType m_type = EDeviceType::Uninitialized;
	vk::Device m_device;

	VKCommandPool m_commandPool;
};