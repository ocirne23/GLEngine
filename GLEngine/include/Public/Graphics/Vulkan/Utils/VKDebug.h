#pragma once

#include "Core.h"
#include "Graphics\Vulkan\vk_cpp.h"

class VKDebug
{
public:

	static const bool VALIDATION_ENABLED = true;

	static const uint VALIDATION_LAYER_COUNT = 1;
	static const char* VALIDATION_LAYER_NAMES[1];

	static void setupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack);
	static void freeDebugCallback(VkInstance instance);

private:

	static PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
	static PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
	static PFN_vkDebugReportMessageEXT dbgBreakCallback;
	static VkDebugReportCallbackEXT msgCallback;
};





