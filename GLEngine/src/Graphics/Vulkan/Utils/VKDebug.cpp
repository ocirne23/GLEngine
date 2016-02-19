#include "Graphics/Vulkan/Utils/VKDebug.h"

const char* VKDebug::VALIDATION_LAYER_NAMES[9] =
{
	"VK_LAYER_LUNARG_threading",
	"VK_LAYER_LUNARG_mem_tracker",
	"VK_LAYER_LUNARG_object_tracker",
	"VK_LAYER_LUNARG_draw_state",
	"VK_LAYER_LUNARG_param_checker",
	"VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_device_limits",
	"VK_LAYER_LUNARG_image",
	"VK_LAYER_GOOGLE_unique_objects",
};

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
	else
		return false;
	return false;
}

END_UNNAMED_NAMESPACE()

void VKDebug::setupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
{
	CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	dbgBreakCallback = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");

	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
	dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgCreateInfo.pNext = NULL;
	dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
	dbgCreateInfo.pUserData = NULL;
	dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	VkDebugReportCallbackEXT debugReportCallback;
	VkResult err = CreateDebugReportCallback(
		instance,
		&dbgCreateInfo,
		NULL,
		&debugReportCallback);
	assert(!err);


}

inline void VKDebug::freeDebugCallback(VkInstance instance)
{
	if (msgCallback != nullptr)
	{
		DestroyDebugReportCallback(instance, msgCallback, nullptr);
	}
}
