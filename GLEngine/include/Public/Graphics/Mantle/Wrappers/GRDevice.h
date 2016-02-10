#pragma once

#include "Core.h"

#include "Graphics/Mantle/Wrappers/GRDeviceQueue.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

class GRPhysicalGPU;

class GRDevice
{
public:
	
	GRDevice() {}
	void initialize(GRPhysicalGPU& gpu, eastl::vector<GRDeviceQueue>& queues, const eastl::vector<eastl::string>& extensions, bool isDebug);

	uint64 getGRDeviceID() const { return m_grDevice; }

private:

	uint64 m_grDevice = 0;
};