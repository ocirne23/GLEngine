#include "Graphics/Mantle/Wrappers/GRDevice.h"

#include "Graphics/Mantle/mantle.h"
#include "Graphics/Mantle/Wrappers/GRPhysicalGPU.h"

#include <assert.h>

void GRDevice::initialize(GRPhysicalGPU& a_gpu, eastl::vector<GRDeviceQueue>& a_queues, const eastl::vector<eastl::string>& a_extensions, bool a_isDebug)
{
	GR_PHYSICAL_GPU gpu = a_gpu.getPhysicalGPUID();

	uint numSupportedExtensions = 0;
	const char** supportedExtensions = new const char*[a_extensions.size()];

	for (uint i = 0; i < a_extensions.size(); ++i)
	{
		const char* extensionName = a_extensions[i].c_str();
		GR_RESULT result = vkGetExtensionSupport(gpu, extensionName);
		if (result == GR_RESULT::GR_SUCCESS)
		{
			supportedExtensions[numSupportedExtensions++] = extensionName;
		}
		else
		{
			print("Unsupported Mantle extension: %s\n", extensionName);
		}
	}

	GR_DEVICE_QUEUE_CREATE_INFO queueInfo[GRDeviceQueue::NUM_QUEUE_TYPES] = {};
	queueInfo[GRDeviceQueue::UNIVERSAL].queueType = GR_QUEUE_UNIVERSAL;
	queueInfo[GRDeviceQueue::COMPUTE].queueType = GR_QUEUE_COMPUTE;

	for (GRDeviceQueue& queue : a_queues)
	{
		switch (queue.getType())
		{
		case GRDeviceQueue::UNIVERSAL:
			queueInfo[GRDeviceQueue::UNIVERSAL].queueCount++;
			break;
		case GRDeviceQueue::COMPUTE:
			queueInfo[GRDeviceQueue::COMPUTE].queueCount++;
			break;
		default:
			assert(false);
			break;
		}
	}

	GR_DEVICE_CREATE_INFO deviceInfo = {};
	deviceInfo.queueRecordCount = GRDeviceQueue::NUM_QUEUE_TYPES;
	deviceInfo.pRequestedQueues = &queueInfo[0];
	deviceInfo.extensionCount = numSupportedExtensions;
	deviceInfo.ppEnabledExtensionNames = supportedExtensions;
	if(a_isDebug)
	{
		deviceInfo.maxValidationLevel = GR_VALIDATION_LEVEL_4;
		deviceInfo.flags |= GR_DEVICE_CREATE_VALIDATION;
	}
	else
	{
		deviceInfo.maxValidationLevel = GR_VALIDATION_LEVEL_0;
	}

	GR_DEVICE device = GR_NULL_HANDLE;
	GR_RESULT result = vkCreateDevice(gpu, &deviceInfo, &device);
	if(result != GR_RESULT::GR_SUCCESS)
	{
		print("Failed to create Mantle device: %i\n", result);
		assert(false);
	}
	m_grDevice = device;

	SAFE_DELETE_ARRAY(supportedExtensions);

	uint queueCount[GRDeviceQueue::NUM_QUEUE_TYPES] = {};
	for (GRDeviceQueue& queue : a_queues)
	{
		GR_QUEUE handle = GR_NULL_HANDLE;
		GR_RESULT queueResult;
		switch (queue.getType())
		{
		case GRDeviceQueue::UNIVERSAL:
			queueResult = vkGetDeviceQueue(device, GR_QUEUE_UNIVERSAL, queueCount[GRDeviceQueue::UNIVERSAL]++, &handle);
			break;
		case GRDeviceQueue::COMPUTE:
			queueResult = vkGetDeviceQueue(device, GR_QUEUE_COMPUTE, queueCount[GRDeviceQueue::COMPUTE]++, &handle);
			break;
		default:
			queueResult = GR_RESULT::GR_ERROR_INVALID_VALUE;
			break;
		}
		if (queueResult == GR_RESULT::GR_SUCCESS)
		{
			queue.onDeviceCreation(handle);
		}
		else
		{
			print("Failed to create Mantle device queue: %i\n", queueResult);
			assert(false);
		}
	}
}
