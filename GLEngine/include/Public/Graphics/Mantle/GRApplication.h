#pragma once

#include "Core.h"

#include "Graphics/Mantle/Wrappers/GRDevice.h"
#include "Graphics/Mantle/Wrappers/GRDeviceQueue.h"
#include "Graphics/Mantle/Wrappers/GRPhysicalGPU.h"

#include "EASTL/vector.h"

class GRApplication
{
public:

	GRApplication() {}
	~GRApplication() {}

	void initialize();
	void shutdown();

private:

	eastl::vector<GRPhysicalGPU> m_gpus;
	eastl::vector<GRDeviceQueue> m_queues;
	eastl::vector<GRDevice> m_devices;
};