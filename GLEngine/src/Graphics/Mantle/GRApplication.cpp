#include "Graphics/Mantle/GRApplication.h"

#include "Graphics/Mantle/mantle.h"

BEGIN_UNNAMED_NAMESPACE()

static const uint NUM_USED_DEVICES = 1;
static const bool DEBUG_MODE = true;

GR_VOID GR_STDCALL debugCallback(GR_ENUM type, GR_ENUM level, GR_BASE_OBJECT obj, GR_SIZE location, GR_ENUM msgCode, const GR_CHAR* msg, GR_VOID* userData)
{
	print("MANTLE: %s\n", msg);
}

END_UNNAMED_NAMESPACE()

void GRApplication::initialize()
{
	if (!mantleLoadFunctions())
	{
		print("Failed to load mantle functions\n");
		return;
	}
	GR_RESULT result = vkDbgRegisterMsgCallback(debugCallback, NULL);
	if (result != GR_SUCCESS)
	{
		print("Failed to register mantle debug callback: %i\n", result);
		return;
	}

	GR_APPLICATION_INFO appInfo = {};
	appInfo.apiVersion = GR_API_VERSION;

	GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS] = {};
	GR_UINT gpuCount = 0;
	result = vkInitAndEnumerateGpus(&appInfo, NULL, &gpuCount, gpus);
	if (result != GR_SUCCESS)
	{
		print("Failed to initialize mantle GPUs: %i\n", result);
		return;
	}
	for (uint i = 0; i < gpuCount; ++i)
	{
		GRPhysicalGPU gpu;
		gpu.initialize(gpus[i]);
		m_gpus.push_back(gpu);
	}
	/* QUEUES */
	m_queues.resize(2);
	m_queues[0].initialize(GRDeviceQueue::EQueueType::UNIVERSAL);
	m_queues[1].initialize(GRDeviceQueue::EQueueType::COMPUTE);

	/* EXTENSIONS */
	eastl::vector<eastl::string> extensions;
	extensions.push_back("GR_WSI_WINDOWS");

	for (uint i = 0; i < m_gpus.size() && i < NUM_USED_DEVICES; ++i)
	{
		GRDevice device;
		device.initialize(m_gpus[i], m_queues, extensions, DEBUG_MODE);
		m_devices.push_back(device);
	}
}

void GRApplication::shutdown()
{

}
