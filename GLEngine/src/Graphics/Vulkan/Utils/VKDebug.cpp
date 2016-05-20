#include "Graphics/Vulkan/Utils/VKDebug.h"

/*
* Vulkan examples debug wrapper
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

const char* VKDebug::VALIDATION_LAYER_NAMES[] =
{
	"VK_LAYER_LUNARG_standard_validation"
};

PFN_vkCreateDebugReportCallbackEXT VKDebug::CreateDebugReportCallback   = NULL;
PFN_vkDestroyDebugReportCallbackEXT VKDebug::DestroyDebugReportCallback = NULL;
PFN_vkDebugReportMessageEXT VKDebug::dbgBreakCallback                   = NULL;
VkDebugReportCallbackEXT VKDebug::msgCallback                           = NULL;

BEGIN_UNNAMED_NAMESPACE()

VkBool32 messageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char * pLayerPrefix, const char * pMsg, void * pUserData)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		print("ERROR: [%s] Code %i : %s\n", pLayerPrefix, msgCode, pMsg);
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		print("WARNING: [%s] Code %i : %s\n", pLayerPrefix, msgCode, pMsg);
	else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		print("INFO: [%s] Code %i : %s\n", pLayerPrefix, msgCode, pMsg);
	else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		print("DEBUG: [%s] Code %i : %s\n", pLayerPrefix, msgCode, pMsg);
	return false;
}

END_UNNAMED_NAMESPACE()

void VKDebug::setupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
{
	CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	dbgBreakCallback = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");

	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
	dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
	dbgCreateInfo.flags = flags;

	VkResult err = CreateDebugReportCallback(
		instance,
		&dbgCreateInfo,
		nullptr,
		&msgCallback);
	assert(!err);
}

inline void VKDebug::freeDebugCallback(VkInstance instance)
{
	if (msgCallback != nullptr)
	{
		// Commented out as this crashes on some implementations for some reason (at least in VS2015)
		// DestroyDebugReportCallback(instance, msgCallback, nullptr);
	}
}
