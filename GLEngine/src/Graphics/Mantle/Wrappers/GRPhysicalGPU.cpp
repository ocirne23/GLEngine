#include "Graphics/Mantle/Wrappers/GRPhysicalGPU.h"

#include "Graphics/Mantle/mantle.h"

void GRPhysicalGPU::initialize(uint64 a_grPhysicalGPUID)
{
	m_grPhysicalGPU = a_grPhysicalGPUID;

	GR_PHYSICAL_GPU_PROPERTIES properties;
	GR_SIZE size = sizeof(GR_PHYSICAL_GPU_PROPERTIES);
	GR_RESULT result = vkGetGpuInfo(m_grPhysicalGPU, GR_INFO_TYPE_PHYSICAL_GPU_PROPERTIES, &size, &properties);
	if (result != GR_SUCCESS)
	{
		
	}

	print("Mantle: Created physical gpu: %s \n", properties.gpuName);
}
