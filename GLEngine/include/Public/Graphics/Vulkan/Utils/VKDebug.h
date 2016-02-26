#pragma once

#include "Core.h"
#include "Graphics\Vulkan\vk_cpp.h"

class VKDebug
{
public:

	static const bool VALIDATION_ENABLED = true;

	static const uint VALIDATION_LAYER_COUNT = 9;
	static const char* VALIDATION_LAYER_NAMES[9];

	void setupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack);
	void freeDebugCallback(VkInstance instance);

private:

	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
	PFN_vkDebugReportMessageEXT dbgBreakCallback;
	VkDebugReportCallbackEXT msgCallback;
};





