#include "Graphics/Mantle/mantle.h"

GR_RESULT(GR_STDCALL *grInitAndEnumerateGpus)        (const GR_APPLICATION_INFO* pAppInfo, const GR_ALLOC_CALLBACKS* pAllocCb, GR_UINT* pGpuCount, GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS]);
GR_RESULT(GR_STDCALL *grGetGpuInfo)                  (GR_PHYSICAL_GPU gpu, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData);
GR_RESULT(GR_STDCALL *grGetExtensionSupport)         (GR_PHYSICAL_GPU gpu, const GR_CHAR* pExtName);
GR_RESULT(GR_STDCALL *grCreateDevice)                (GR_PHYSICAL_GPU gpu, const GR_DEVICE_CREATE_INFO* pCreateInfo, GR_DEVICE* pDevice);
GR_RESULT(GR_STDCALL *grGetDeviceQueue)              (GR_DEVICE device, GR_ENUM queueType, GR_UINT queueId, GR_QUEUE* pQueue);
GR_RESULT(GR_STDCALL *grCreateCommandBuffer)         (GR_DEVICE device, const GR_CMD_BUFFER_CREATE_INFO* pCreateInfo, GR_CMD_BUFFER* pCmdBuffer);
GR_RESULT(GR_STDCALL *grBeginCommandBuffer)          (GR_CMD_BUFFER cmdBuffer, GR_FLAGS flags);
GR_RESULT(GR_STDCALL *grEndCommandBuffer)            (GR_CMD_BUFFER cmdBuffer);
GR_RESULT(GR_STDCALL *grQueueSubmit)                 (GR_QUEUE queue, GR_UINT cmdBufferCount, const GR_CMD_BUFFER* pCmdBuffers, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs, GR_FENCE fence);
  GR_VOID(GR_STDCALL *grCmdPrepareImages)            (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_IMAGE_STATE_TRANSITION* pStateTransitions);
  GR_VOID(GR_STDCALL *grCmdClearColorImage)          (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_FLOAT color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges);
GR_RESULT(GR_STDCALL *grWsiWinQueuePresent)          (GR_QUEUE queue, const GR_WSI_WIN_PRESENT_INFO* pPresentInfo);
GR_RESULT(GR_STDCALL *grDbgRegisterMsgCallback)      (GR_DBG_MSG_CALLBACK_FUNCTION pfnMsgCallback, GR_VOID* pUserData);
GR_RESULT(GR_STDCALL *grCreateViewportState)         (GR_DEVICE device, const GR_VIEWPORT_STATE_CREATE_INFO* pCreateInfo, GR_VIEWPORT_STATE_OBJECT* pState);
GR_RESULT(GR_STDCALL *grGetMemoryHeapCount)          (GR_DEVICE device, GR_UINT* pCount);
GR_RESULT(GR_STDCALL *grCreateColorTargetView)       (GR_DEVICE device, const GR_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo, GR_COLOR_TARGET_VIEW* pView);
GR_RESULT(GR_STDCALL *grGetMemoryHeapInfo)           (GR_DEVICE device, GR_UINT heapId, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData);
GR_RESULT(GR_STDCALL *grAllocMemory)                 (GR_DEVICE device, const GR_MEMORY_ALLOC_INFO* pAllocInfo, GR_GPU_MEMORY* pMem);
GR_RESULT(GR_STDCALL *grCreateDescriptorSet)         (GR_DEVICE device, const GR_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo, GR_DESCRIPTOR_SET* pDescriptorSet);
GR_RESULT(GR_STDCALL *grGetObjectInfo)               (GR_BASE_OBJECT object, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData);
GR_RESULT(GR_STDCALL *grBindObjectMemory)            (GR_OBJECT object, GR_GPU_MEMORY mem, GR_GPU_SIZE offset);
GR_RESULT(GR_STDCALL *grCreateFence)                 (GR_DEVICE device, const GR_FENCE_CREATE_INFO* pCreateInfo, GR_FENCE* pFence);
GR_RESULT(GR_STDCALL *grCreateMsaaState)             (GR_DEVICE device, const GR_MSAA_STATE_CREATE_INFO* pCreateInfo, GR_MSAA_STATE_OBJECT* pState);
GR_RESULT(GR_STDCALL *grCreateSampler)               (GR_DEVICE device, const GR_SAMPLER_CREATE_INFO* pCreateInfo, GR_SAMPLER* pSampler);
  GR_VOID(GR_STDCALL *grBeginDescriptorSetUpdate)    (GR_DESCRIPTOR_SET descriptorSet);
  GR_VOID(GR_STDCALL *grEndDescriptorSetUpdate)      (GR_DESCRIPTOR_SET descriptorSet);
  GR_VOID(GR_STDCALL *grAttachSamplerDescriptors)    (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_SAMPLER* pSamplers);
GR_RESULT(GR_STDCALL *grCreateColorBlendState)       (GR_DEVICE device, const GR_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo, GR_COLOR_BLEND_STATE_OBJECT* pState);
GR_RESULT(GR_STDCALL *grCreateDepthStencilState)     (GR_DEVICE device, const GR_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo, GR_DEPTH_STENCIL_STATE_OBJECT* pState);
GR_RESULT(GR_STDCALL *grCreateRasterState)           (GR_DEVICE device, const GR_RASTER_STATE_CREATE_INFO* pCreateInfo, GR_RASTER_STATE_OBJECT* pState);
GR_RESULT(GR_STDCALL *grMapMemory)                   (GR_GPU_MEMORY mem, GR_FLAGS flags, GR_VOID** ppData);
GR_RESULT(GR_STDCALL *grUnmapMemory)                 (GR_GPU_MEMORY mem);
GR_RESULT(GR_STDCALL *grCreateShader)                (GR_DEVICE device, const GR_SHADER_CREATE_INFO* pCreateInfo, GR_SHADER* pShader);
GR_RESULT(GR_STDCALL *grCreateGraphicsPipeline)      (GR_DEVICE device, const GR_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline);
GR_RESULT(GR_STDCALL *grCreateComputePipeline)       (GR_DEVICE device, const GR_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline);
  GR_VOID(GR_STDCALL *grClearDescriptorSetSlots)     (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount);
  GR_VOID(GR_STDCALL *grAttachMemoryViewDescriptors) (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_MEMORY_VIEW_ATTACH_INFO* pMemViews);
GR_RESULT(GR_STDCALL *grWaitForFences)               (GR_DEVICE device, GR_UINT fenceCount, const GR_FENCE* pFences, GR_BOOL waitAll, GR_FLOAT timeout);
GR_RESULT(GR_STDCALL *grCreateImage)                 (GR_DEVICE device, const GR_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage);
GR_RESULT(GR_STDCALL *grDestroyObject)               (GR_OBJECT object);
GR_RESULT(GR_STDCALL *grCreateImageView)             (GR_DEVICE device, const GR_IMAGE_VIEW_CREATE_INFO* pCreateInfo, GR_IMAGE_VIEW* pView);
  GR_VOID(GR_STDCALL *grAttachImageViewDescriptors)  (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_IMAGE_VIEW_ATTACH_INFO* pImageViews);
GR_RESULT(GR_STDCALL *grGetImageSubresourceInfo)     (GR_IMAGE image, const GR_IMAGE_SUBRESOURCE* pSubresource, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData);
GR_RESULT(GR_STDCALL *grResetCommandBuffer)          (GR_CMD_BUFFER cmdBuffer);
  GR_VOID(GR_STDCALL *grCmdPrepareMemoryRegions)     (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_MEMORY_STATE_TRANSITION* pStateTransitions);
  GR_VOID(GR_STDCALL *grCmdCopyMemory)               (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_COPY* pRegions);
  GR_VOID(GR_STDCALL *grCmdCopyImage)                (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_COPY* pRegions);
  GR_VOID(GR_STDCALL *grCmdBindTargets)              (GR_CMD_BUFFER cmdBuffer, GR_UINT colorTargetCount, const GR_COLOR_TARGET_BIND_INFO* pColorTargets, const GR_DEPTH_STENCIL_BIND_INFO* pDepthTarget);
  GR_VOID(GR_STDCALL *grCmdBindStateObject)          (GR_CMD_BUFFER cmdBuffer, GR_ENUM stateBindPoint, GR_STATE_OBJECT state);
GR_RESULT(GR_STDCALL *grFreeMemory)                  (GR_GPU_MEMORY mem);
  GR_VOID(GR_STDCALL *grCmdCopyMemoryToImage)        (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_IMAGE destImage, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions);
  GR_VOID(GR_STDCALL *grAttachNestedDescriptors)     (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets);
  GR_VOID(GR_STDCALL *grCmdBindDescriptorSet)        (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT index, GR_DESCRIPTOR_SET descriptorSet, GR_UINT slotOffset);
  GR_VOID(GR_STDCALL *grCmdBindIndexData)            (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset, GR_ENUM indexType);
  GR_VOID(GR_STDCALL *grCmdBindPipeline)             (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_PIPELINE pipeline);
  GR_VOID(GR_STDCALL *grCmdDrawIndexed)              (GR_CMD_BUFFER cmdBuffer, GR_UINT firstIndex, GR_UINT indexCount, GR_INT vertexOffset, GR_UINT firstInstance, GR_UINT instanceCount);
  GR_VOID(GR_STDCALL *grCmdBindDynamicMemoryView)    (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, const GR_MEMORY_VIEW_ATTACH_INFO* pMemView);
  GR_VOID(GR_STDCALL *grCmdDispatch)                 (GR_CMD_BUFFER cmdBuffer, GR_UINT x, GR_UINT y, GR_UINT z);
  GR_VOID(GR_STDCALL *grCmdDraw)                     (GR_CMD_BUFFER cmdBuffer, GR_UINT firstVertex, GR_UINT vertexCount, GR_UINT firstInstance, GR_UINT instanceCount);
GR_RESULT(GR_STDCALL *grWsiWinGetDisplays)           (GR_DEVICE device, GR_UINT* pDisplayCount, GR_WSI_WIN_DISPLAY* pDisplayList);
GR_RESULT(GR_STDCALL *grWsiWinGetDisplayModeList)    (GR_WSI_WIN_DISPLAY display, GR_UINT* pDisplayModeCount, GR_WSI_WIN_DISPLAY_MODE* pDisplayModeList);
GR_RESULT(GR_STDCALL *grWsiWinCreatePresentableImage)(GR_DEVICE device, const GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem);

#define LOAD_PROC(fnCheck, module, var)\
{\
    var = (decltype(var)) GetProcAddress(module, #var);\
    if (!var) fnCheck = false;\
}\

// Applications should call this at the beginning of the program
bool mantleLoadFunctions()
{
	static bool loaded = false;
	if (loaded) return true;
	loaded = true;

	HMODULE mantleDll = LoadLibrary(TEXT("C:/Windows/System32/mantle64.dll"));
	if (!mantleDll) return false;

	bool fnCheck = true;

	LOAD_PROC(fnCheck, mantleDll, grCreateViewportState);
	LOAD_PROC(fnCheck, mantleDll, grDbgRegisterMsgCallback);
	LOAD_PROC(fnCheck, mantleDll, grInitAndEnumerateGpus);
	LOAD_PROC(fnCheck, mantleDll, grGetGpuInfo);
	LOAD_PROC(fnCheck, mantleDll, grGetExtensionSupport);
	LOAD_PROC(fnCheck, mantleDll, grCreateDevice);
	LOAD_PROC(fnCheck, mantleDll, grGetDeviceQueue);
	LOAD_PROC(fnCheck, mantleDll, grCreateCommandBuffer);
	LOAD_PROC(fnCheck, mantleDll, grBeginCommandBuffer);
	LOAD_PROC(fnCheck, mantleDll, grEndCommandBuffer);
	LOAD_PROC(fnCheck, mantleDll, grQueueSubmit);
	LOAD_PROC(fnCheck, mantleDll, grCmdPrepareImages);
	LOAD_PROC(fnCheck, mantleDll, grCmdClearColorImage);
	LOAD_PROC(fnCheck, mantleDll, grGetMemoryHeapCount);
	LOAD_PROC(fnCheck, mantleDll, grCreateColorTargetView);
	LOAD_PROC(fnCheck, mantleDll, grGetMemoryHeapInfo);
	LOAD_PROC(fnCheck, mantleDll, grAllocMemory);
	LOAD_PROC(fnCheck, mantleDll, grCreateDescriptorSet);
	LOAD_PROC(fnCheck, mantleDll, grGetObjectInfo);
	LOAD_PROC(fnCheck, mantleDll, grBindObjectMemory);
	LOAD_PROC(fnCheck, mantleDll, grCreateFence);
	LOAD_PROC(fnCheck, mantleDll, grCreateMsaaState);
	LOAD_PROC(fnCheck, mantleDll, grCreateSampler);
	LOAD_PROC(fnCheck, mantleDll, grBeginDescriptorSetUpdate);
	LOAD_PROC(fnCheck, mantleDll, grEndDescriptorSetUpdate);
	LOAD_PROC(fnCheck, mantleDll, grAttachSamplerDescriptors);
	LOAD_PROC(fnCheck, mantleDll, grCreateColorBlendState);
	LOAD_PROC(fnCheck, mantleDll, grCreateDepthStencilState);
	LOAD_PROC(fnCheck, mantleDll, grCreateRasterState);
	LOAD_PROC(fnCheck, mantleDll, grMapMemory);
	LOAD_PROC(fnCheck, mantleDll, grUnmapMemory);
	LOAD_PROC(fnCheck, mantleDll, grCreateShader);
	LOAD_PROC(fnCheck, mantleDll, grCreateGraphicsPipeline);
	LOAD_PROC(fnCheck, mantleDll, grCreateComputePipeline);
	LOAD_PROC(fnCheck, mantleDll, grClearDescriptorSetSlots);
	LOAD_PROC(fnCheck, mantleDll, grAttachMemoryViewDescriptors);
	LOAD_PROC(fnCheck, mantleDll, grWaitForFences);
	LOAD_PROC(fnCheck, mantleDll, grCreateImage);
	LOAD_PROC(fnCheck, mantleDll, grDestroyObject);
	LOAD_PROC(fnCheck, mantleDll, grCreateImageView);
	LOAD_PROC(fnCheck, mantleDll, grAttachImageViewDescriptors);
	LOAD_PROC(fnCheck, mantleDll, grGetImageSubresourceInfo);
	LOAD_PROC(fnCheck, mantleDll, grResetCommandBuffer);
	LOAD_PROC(fnCheck, mantleDll, grCmdPrepareMemoryRegions);
	LOAD_PROC(fnCheck, mantleDll, grCmdCopyMemory);
	LOAD_PROC(fnCheck, mantleDll, grCmdCopyImage);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindTargets);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindStateObject);
	LOAD_PROC(fnCheck, mantleDll, grFreeMemory);
	LOAD_PROC(fnCheck, mantleDll, grCmdCopyMemoryToImage);
	LOAD_PROC(fnCheck, mantleDll, grAttachNestedDescriptors);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindDescriptorSet);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindIndexData);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindPipeline);
	LOAD_PROC(fnCheck, mantleDll, grCmdDrawIndexed);
	LOAD_PROC(fnCheck, mantleDll, grCmdBindDynamicMemoryView);
	LOAD_PROC(fnCheck, mantleDll, grCmdDispatch);
	LOAD_PROC(fnCheck, mantleDll, grCmdDraw);

	LOAD_PROC(fnCheck, mantleDll, grWsiWinGetDisplays);
	LOAD_PROC(fnCheck, mantleDll, grWsiWinGetDisplayModeList);
	LOAD_PROC(fnCheck, mantleDll, grWsiWinCreatePresentableImage);
	LOAD_PROC(fnCheck, mantleDll, grWsiWinQueuePresent);

	return fnCheck;
}

GR_RESULT vkInitAndEnumerateGpus(const GR_APPLICATION_INFO* pAppInfo, const GR_ALLOC_CALLBACKS* pAllocCb, GR_UINT* pGpuCount, GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS]) 
{
	return grInitAndEnumerateGpus(pAppInfo, pAllocCb, pGpuCount, gpus);
}
GR_RESULT vkGetGpuInfo(GR_PHYSICAL_GPU gpu, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData) 
{
	return grGetGpuInfo(gpu, infoType, pDataSize, pData);
}
GR_RESULT vkGetExtensionSupport(GR_PHYSICAL_GPU gpu, const GR_CHAR* pExtName) 
{
	return grGetExtensionSupport(gpu, pExtName);
}
GR_RESULT vkCreateDevice(GR_PHYSICAL_GPU gpu, const GR_DEVICE_CREATE_INFO* pCreateInfo, GR_DEVICE* pDevice)
{
	return grCreateDevice(gpu, pCreateInfo, pDevice);
}
GR_RESULT vkGetDeviceQueue(GR_DEVICE device, GR_ENUM queueType, GR_UINT queueId, GR_QUEUE* pQueue)
{
	return grGetDeviceQueue(device, queueType, queueId, pQueue);
}
GR_RESULT vkCreateCommandBuffer(GR_DEVICE device, const GR_CMD_BUFFER_CREATE_INFO* pCreateInfo, GR_CMD_BUFFER* pCmdBuffer)
{
	return grCreateCommandBuffer(device, pCreateInfo, pCmdBuffer);
}
GR_RESULT vkBeginCommandBuffer(GR_CMD_BUFFER cmdBuffer, GR_FLAGS flags)
{
	return grBeginCommandBuffer(cmdBuffer, flags);
}
GR_RESULT vkEndCommandBuffer(GR_CMD_BUFFER cmdBuffer)
{
	return grEndCommandBuffer(cmdBuffer);
}
GR_RESULT vkQueueSubmit(GR_QUEUE queue, GR_UINT cmdBufferCount, const GR_CMD_BUFFER* pCmdBuffers, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs, GR_FENCE fence)
{
	return grQueueSubmit(queue, cmdBufferCount, pCmdBuffers, memRefCount,pMemRefs, fence);
}
GR_VOID vkCmdPrepareImages(GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_IMAGE_STATE_TRANSITION* pStateTransitions)
{
	grCmdPrepareImages(cmdBuffer, transitionCount, pStateTransitions);
}
GR_VOID vkCmdClearColorImage(GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_FLOAT color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges)
{
	grCmdClearColorImage(cmdBuffer, image, color, rangeCount, pRanges);
}
GR_RESULT vkWsiWinQueuePresent(GR_QUEUE queue, const GR_WSI_WIN_PRESENT_INFO* pPresentInfo)
{
	return grWsiWinQueuePresent(queue, pPresentInfo);
}
GR_RESULT vkDbgRegisterMsgCallback(GR_DBG_MSG_CALLBACK_FUNCTION pfnMsgCallback, GR_VOID* pUserData)
{
	return grDbgRegisterMsgCallback(pfnMsgCallback, pUserData);
}
GR_RESULT vkCreateViewportState(GR_DEVICE device, const GR_VIEWPORT_STATE_CREATE_INFO* pCreateInfo, GR_VIEWPORT_STATE_OBJECT* pState)
{
	return grCreateViewportState(device, pCreateInfo, pState);
}
GR_RESULT vkGetMemoryHeapCount(GR_DEVICE device, GR_UINT* pCount)
{
	return grGetMemoryHeapCount(device, pCount);
}
GR_RESULT vkCreateColorTargetView(GR_DEVICE device, const GR_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo, GR_COLOR_TARGET_VIEW* pView)
{
	return grCreateColorTargetView(device, pCreateInfo, pView);
}
GR_RESULT vkGetMemoryHeapInfo(GR_DEVICE device, GR_UINT heapId, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData)
{
	return grGetMemoryHeapInfo(device, heapId, infoType, pDataSize, pData);
}
GR_RESULT vkAllocMemory(GR_DEVICE device, const GR_MEMORY_ALLOC_INFO* pAllocInfo, GR_GPU_MEMORY* pMem)
{
	return grAllocMemory(device, pAllocInfo, pMem);
}
GR_RESULT vkCreateDescriptorSet(GR_DEVICE device, const GR_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo, GR_DESCRIPTOR_SET* pDescriptorSet)
{
	return grCreateDescriptorSet(device, pCreateInfo, pDescriptorSet);
}
GR_RESULT vkGetObjectInfo(GR_BASE_OBJECT object, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData)
{
	return grGetObjectInfo(object, infoType, pDataSize, pData);
}
GR_RESULT vkBindObjectMemory(GR_OBJECT object, GR_GPU_MEMORY mem, GR_GPU_SIZE offset)
{
	return grBindObjectMemory(object, mem, offset);
}
GR_RESULT vkCreateFence(GR_DEVICE device, const GR_FENCE_CREATE_INFO* pCreateInfo, GR_FENCE* pFence)
{
	return grCreateFence(device, pCreateInfo, pFence);
}
GR_RESULT vkCreateMsaaState(GR_DEVICE device, const GR_MSAA_STATE_CREATE_INFO* pCreateInfo, GR_MSAA_STATE_OBJECT* pState)
{
	return grCreateMsaaState(device, pCreateInfo, pState);
}
GR_RESULT vkCreateSampler(GR_DEVICE device, const GR_SAMPLER_CREATE_INFO* pCreateInfo, GR_SAMPLER* pSampler)
{
	return grCreateSampler(device, pCreateInfo, pSampler);
}
GR_VOID vkBeginDescriptorSetUpdate(GR_DESCRIPTOR_SET descriptorSet)
{
	grBeginDescriptorSetUpdate(descriptorSet);
}
GR_VOID vkEndDescriptorSetUpdate(GR_DESCRIPTOR_SET descriptorSet)
{
	grEndDescriptorSetUpdate(descriptorSet);
}
GR_VOID vkAttachSamplerDescriptors(GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_SAMPLER* pSamplers)
{
	grAttachSamplerDescriptors(descriptorSet, startSlot, slotCount, pSamplers);
}
GR_RESULT vkCreateColorBlendState(GR_DEVICE device, const GR_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo, GR_COLOR_BLEND_STATE_OBJECT* pState)
{
	return grCreateColorBlendState(device, pCreateInfo, pState);
}
GR_RESULT vkCreateDepthStencilState(GR_DEVICE device, const GR_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo, GR_DEPTH_STENCIL_STATE_OBJECT* pState)
{
	return grCreateDepthStencilState(device, pCreateInfo, pState);
}
GR_RESULT vkCreateRasterState(GR_DEVICE device, const GR_RASTER_STATE_CREATE_INFO* pCreateInfo, GR_RASTER_STATE_OBJECT* pState)
{
	return grCreateRasterState(device, pCreateInfo, pState);
}
GR_RESULT vkMapMemory(GR_GPU_MEMORY mem, GR_FLAGS flags, GR_VOID** ppData)
{
	return grMapMemory(mem, flags, ppData);
}
GR_RESULT vkUnmapMemory(GR_GPU_MEMORY mem)
{
	return grUnmapMemory(mem);
}
GR_RESULT vkCreateShader(GR_DEVICE device, const GR_SHADER_CREATE_INFO* pCreateInfo, GR_SHADER* pShader)
{
	return grCreateShader(device, pCreateInfo, pShader);
}
GR_RESULT vkCreateGraphicsPipeline(GR_DEVICE device, const GR_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline)
{
	return grCreateGraphicsPipeline(device, pCreateInfo, pPipeline);
}
GR_RESULT vkCreateComputePipeline(GR_DEVICE device, const GR_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline)
{
	return grCreateComputePipeline(device, pCreateInfo, pPipeline);
}
GR_VOID vkClearDescriptorSetSlots(GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount)
{
	grClearDescriptorSetSlots(descriptorSet, startSlot, slotCount);
}
GR_VOID vkAttachMemoryViewDescriptors(GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_MEMORY_VIEW_ATTACH_INFO* pMemViews)
{
	grAttachMemoryViewDescriptors(descriptorSet, startSlot, slotCount, pMemViews);
}
GR_RESULT vkWaitForFences(GR_DEVICE device, GR_UINT fenceCount, const GR_FENCE* pFences, GR_BOOL waitAll, GR_FLOAT timeout)
{
	return grWaitForFences(device, fenceCount, pFences, waitAll, timeout);
}
GR_RESULT vkCreateImage(GR_DEVICE device, const GR_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage)
{
	return grCreateImage(device, pCreateInfo, pImage);
}
GR_RESULT vkDestroyObject(GR_OBJECT object)
{
	return grDestroyObject(object);
}
GR_RESULT vkCreateImageView(GR_DEVICE device, const GR_IMAGE_VIEW_CREATE_INFO* pCreateInfo, GR_IMAGE_VIEW* pView)
{
	return grCreateImageView(device, pCreateInfo, pView);
}
GR_VOID vkAttachImageViewDescriptors(GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_IMAGE_VIEW_ATTACH_INFO* pImageViews)
{
	grAttachImageViewDescriptors(descriptorSet, startSlot, slotCount, pImageViews);
}
GR_RESULT vkGetImageSubresourceInfo(GR_IMAGE image, const GR_IMAGE_SUBRESOURCE* pSubresource, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData)
{
	return grGetImageSubresourceInfo(image, pSubresource, infoType, pDataSize, pData);
}
GR_RESULT vkResetCommandBuffer(GR_CMD_BUFFER cmdBuffer)
{
	return grResetCommandBuffer(cmdBuffer);
}
GR_VOID vkCmdPrepareMemoryRegions(GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_MEMORY_STATE_TRANSITION* pStateTransitions)
{
	grCmdPrepareMemoryRegions(cmdBuffer, transitionCount, pStateTransitions);
}
GR_VOID vkCmdCopyMemory(GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_COPY* pRegions)
{
	grCmdCopyMemory(cmdBuffer, srcMem, destMem, regionCount, pRegions);
}
GR_VOID vkCmdCopyImage(GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_COPY* pRegions)
{
	grCmdCopyImage(cmdBuffer, srcImage, destImage, regionCount, pRegions);
}
GR_VOID vkCmdBindTargets(GR_CMD_BUFFER cmdBuffer, GR_UINT colorTargetCount, const GR_COLOR_TARGET_BIND_INFO* pColorTargets, const GR_DEPTH_STENCIL_BIND_INFO* pDepthTarget)
{
	grCmdBindTargets(cmdBuffer, colorTargetCount, pColorTargets, pDepthTarget);
}
GR_VOID vkCmdBindStateObject(GR_CMD_BUFFER cmdBuffer, GR_ENUM stateBindPoint, GR_STATE_OBJECT state)
{
	grCmdBindStateObject(cmdBuffer, stateBindPoint, state);
}
GR_RESULT vkFreeMemory(GR_GPU_MEMORY mem)
{
	return grFreeMemory(mem);
}
GR_VOID vkCmdCopyMemoryToImage(GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_IMAGE destImage, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions)
{
	grCmdCopyMemoryToImage(cmdBuffer, srcMem, destImage, regionCount, pRegions);
}
GR_VOID vkAttachNestedDescriptors(GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets)
{
	grAttachNestedDescriptors(descriptorSet, startSlot, slotCount, pNestedDescriptorSets);
}
GR_VOID vkCmdBindDescriptorSet(GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT index, GR_DESCRIPTOR_SET descriptorSet, GR_UINT slotOffset)
{
	grCmdBindDescriptorSet(cmdBuffer, pipelineBindPoint, index, descriptorSet, slotOffset);
}
GR_VOID vkCmdBindIndexData(GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset, GR_ENUM indexType)
{
	grCmdBindIndexData(cmdBuffer, mem, offset, indexType);
}
GR_VOID vkCmdBindPipeline(GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_PIPELINE pipeline)
{
	grCmdBindPipeline(cmdBuffer, pipelineBindPoint, pipeline);
}
GR_VOID vkCmdDrawIndexed(GR_CMD_BUFFER cmdBuffer, GR_UINT firstIndex, GR_UINT indexCount, GR_INT vertexOffset, GR_UINT firstInstance, GR_UINT instanceCount)
{
	grCmdDrawIndexed(cmdBuffer, firstIndex, indexCount, vertexOffset, firstInstance, instanceCount);
}
GR_VOID vkCmdBindDynamicMemoryView(GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, const GR_MEMORY_VIEW_ATTACH_INFO* pMemView) 
{
	grCmdBindDynamicMemoryView(cmdBuffer, pipelineBindPoint, pMemView);
}
GR_VOID vkCmdDispatch(GR_CMD_BUFFER cmdBuffer, GR_UINT x, GR_UINT y, GR_UINT z) 
{
	grCmdDispatch(cmdBuffer, x, y, z);
}
GR_VOID vkCmdDraw(GR_CMD_BUFFER cmdBuffer, GR_UINT firstVertex, GR_UINT vertexCount, GR_UINT firstInstance, GR_UINT instanceCount) 
{
	grCmdDraw(cmdBuffer, firstVertex, vertexCount, firstInstance, instanceCount);
}
GR_RESULT vkWsiWinGetDisplays(GR_DEVICE device, GR_UINT* pDisplayCount, GR_WSI_WIN_DISPLAY* pDisplayList) 
{
	return grWsiWinGetDisplays(device, pDisplayCount, pDisplayList);
}
GR_RESULT vkWsiWinGetDisplayModeList(GR_WSI_WIN_DISPLAY display, GR_UINT* pDisplayModeCount, GR_WSI_WIN_DISPLAY_MODE* pDisplayModeList) 
{
	return grWsiWinGetDisplayModeList(display, pDisplayModeCount, pDisplayModeList);
}
GR_RESULT vkWsiWinCreatePresentableImage(GR_DEVICE device, const GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem) 
{
	return grWsiWinCreatePresentableImage(device, pCreateInfo, pImage, pMem);
}
