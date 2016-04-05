#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKPhysicalDevice;

class VKDevice
{
public:

	enum class EDeviceType { Graphics, Compute, Transfer, SparseBinding, NUM_TYPES, Uninitialized };

public:

	VKDevice() {}
	VKDevice(const VKDevice& copy) { assert(!m_initialized); }
	~VKDevice();

	bool isInitialized() const { return m_initialized; }

	vk::Device getVKDevice() 
	{ 
		assert(m_initialized); 
		return m_device; 
	}

	vk::Queue getVKQueue()
	{
		assert(m_initialized);
		return m_queue;
	}

	vk::CommandPool getVKCommandPool()
	{
		assert(m_initialized);
		return m_commandPool;
	}

	VKPhysicalDevice* getPhysDevice() 
	{ 
		assert(m_initialized); 
		return m_physDevice; 
	}

private:

	friend class VKPhysicalDevice;

	void initialize(VKPhysicalDevice& physDevice, EDeviceType type);
	void cleanup();

private:

	bool m_initialized = false;
	VKPhysicalDevice* m_physDevice;
	EDeviceType m_type = EDeviceType::Uninitialized;
	vk::Device m_device;
	vk::Queue m_queue;
	vk::CommandPool m_commandPool;
};