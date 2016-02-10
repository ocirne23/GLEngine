/***********************************************************************//*
Copyright (c) 2015 Alexander Overvoorde

Permission is hereby vkanted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
/************************************************************************/

#include "Graphics/Mantle/MantleTest.h"

#include "Graphics/Mantle/mantle.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <SDL/SDL.h>

const int WIDTH = 1280;
const int HEIGHT = 720;

float vertices[] = {
	// Positions
	0.0, 0.5, 0.0, 1.0,
	0.5, -0.5, 0.0, 1.0,
	-0.5, -0.5, 0.0, 1.0,

	// Colors
	1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0
};

GR_CMD_BUFFER_CREATE_INFO bufferCreateInfo = { GR_QUEUE_UNIVERSAL, 0 };

GR_MSAA_STATE_OBJECT msaaState;
GR_VIEWPORT_STATE_OBJECT viewportState;
GR_COLOR_BLEND_STATE_OBJECT colorBlendState;
GR_DEPTH_STENCIL_STATE_OBJECT depthStencilState;
GR_RASTER_STATE_OBJECT rasterState;

GR_VOID GR_STDCALL debugCallback(GR_ENUM type, GR_ENUM level, GR_BASE_OBJECT obj, GR_SIZE location, GR_ENUM msgCode, const GR_CHAR* msg, GR_VOID* userData)
{
	std::cerr << "DEBUG: " << msg << std::endl;
}

std::vector<char> loadShader(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	auto size = file.tellg();
	file.seekg(std::ios::beg, 0);

	std::vector<char> data;
	data.resize(size);

	file.read(&data[0], size);

	file.close();

	return data;
}

GR_MEMORY_REF allocateObjectMemory(GR_DEVICE device, GR_OBJECT object)
{
	GR_MEMORY_REQUIREMENTS memReqs = {};
	GR_SIZE memReqsSize = sizeof(memReqs);
	vkGetObjectInfo(object, GR_INFO_TYPE_MEMORY_REQUIREMENTS, &memReqsSize, &memReqs);

	GR_MEMORY_HEAP_PROPERTIES heapProps = {};
	GR_SIZE heapPropsSize = sizeof(heapProps);
	vkGetMemoryHeapInfo(device, memReqs.heaps[0], GR_INFO_TYPE_MEMORY_HEAP_PROPERTIES, &heapPropsSize, &heapProps);

	GR_GPU_MEMORY memory;
	GR_MEMORY_ALLOC_INFO allocInfo = {};
	allocInfo.size = (1 + memReqs.size / heapProps.pageSize) * heapProps.pageSize;
	allocInfo.alignment = 0;
	allocInfo.memPriority = GR_MEMORY_PRIORITY_HIGH;
	allocInfo.heapCount = 1;
	allocInfo.heaps[0] = memReqs.heaps[0];
	vkAllocMemory(device, &allocInfo, &memory);

	vkBindObjectMemory(object, memory, 0);

	GR_MEMORY_REF memoryRef = {};
	memoryRef.mem = memory;

	return memoryRef;
}

GR_GPU_MEMORY allocateMappableBuffer(GR_DEVICE device, GR_GPU_SIZE size)
{
	// Find CPU visible (mappable) heap
	GR_UINT heapCount;
	vkGetMemoryHeapCount(device, &heapCount);

	GR_MEMORY_HEAP_PROPERTIES heapProps = {};
	GR_SIZE heapPropsSize = sizeof(heapProps);
	GR_UINT suitableHeap = -1;

	for (unsigned int i = 0; i < heapCount; i++)
	{
		vkGetMemoryHeapInfo(device, i, GR_INFO_TYPE_MEMORY_HEAP_PROPERTIES, &heapPropsSize, &heapProps);

		if (heapProps.flags & GR_MEMORY_HEAP_CPU_VISIBLE)
		{
			suitableHeap = i;
			break;
		}
	}

	// Allocate buffer in that heap with multiple of page size >= size
	GR_GPU_MEMORY memory;
	GR_MEMORY_ALLOC_INFO allocInfo = {};
	allocInfo.size = (1 + size / heapProps.pageSize) * heapProps.pageSize;
	allocInfo.alignment = 0;
	allocInfo.memPriority = GR_MEMORY_PRIORITY_HIGH;
	allocInfo.heapCount = 1;
	allocInfo.heaps[0] = suitableHeap;
	vkAllocMemory(device, &allocInfo, &memory);

	return memory;
}

void createDeviceAndQueue(GR_DEVICE& device, GR_QUEUE& universalQueue)
{
	// Find Mantle compatible GPU handles
	GR_APPLICATION_INFO appInfo = {};
	appInfo.apiVersion = GR_API_VERSION;

	GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS] = {};
	GR_UINT gpuCount = 0;

	vkInitAndEnumerateGpus(&appInfo, nullptr, &gpuCount, gpus);

	// Create device from first compatible GPU
	GR_DEVICE_QUEUE_CREATE_INFO queueInfo = {};
	queueInfo.queueType = GR_QUEUE_UNIVERSAL;
	queueInfo.queueCount = 1;

	assert(vkGetExtensionSupport(gpus[0], "GR_WSI_WINDOWS") == GR_SUCCESS);

	static const GR_CHAR* const ppExtensions[] = {
		"GR_WSI_WINDOWS",
	};

	GR_DEVICE_CREATE_INFO deviceInfo = {};
	deviceInfo.queueRecordCount = 1;
	deviceInfo.pRequestedQueues = &queueInfo;
	deviceInfo.extensionCount = 1;
	deviceInfo.ppEnabledExtensionNames = ppExtensions;
	deviceInfo.flags |= GR_DEVICE_CREATE_VALIDATION;
	deviceInfo.maxValidationLevel = GR_VALIDATION_LEVEL_4;

	vkCreateDevice(gpus[0], &deviceInfo, &device);

	vkGetDeviceQueue(device, GR_QUEUE_UNIVERSAL, 0, &universalQueue);
}

void initPresentableImage(GR_DEVICE device, GR_QUEUE commandQueue, GR_IMAGE& image, GR_MEMORY_REF& imageMemoryRef, GR_IMAGE_SUBRESOURCE_RANGE& imageColorRange)
{
	// Create image that can be presented
	GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO imageCreateInfo = {};
	imageCreateInfo.format = {
		GR_CH_FMT_R8G8B8A8,
		GR_NUM_FMT_UNORM
	};
	imageCreateInfo.usage = GR_IMAGE_USAGE_COLOR_TARGET;
	imageCreateInfo.extent = { WIDTH, HEIGHT };

	GR_GPU_MEMORY imageMemory;
	vkWsiWinCreatePresentableImage(device, &imageCreateInfo, &image, &imageMemory);

	imageMemoryRef = {};
	imageMemoryRef.mem = imageMemory;

	imageColorRange = {};
	imageColorRange.aspect = GR_IMAGE_ASPECT_COLOR;
	imageColorRange.baseMipLevel = 0;
	imageColorRange.mipLevels = 1;
	imageColorRange.baseArraySlice = 0;
	imageColorRange.arraySize = 1;

	// Create and submit command buffer that transitions the image to being presentable
	GR_CMD_BUFFER initCmdBuffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &initCmdBuffer);

	vkBeginCommandBuffer(initCmdBuffer, 0);

	GR_IMAGE_STATE_TRANSITION initTransition = {};
	initTransition.image = image;
	initTransition.oldState = GR_IMAGE_STATE_UNINITIALIZED;
	initTransition.newState = GR_WSI_WIN_IMAGE_STATE_PRESENT_WINDOWED;
	initTransition.subresourceRange = imageColorRange;

	vkCmdPrepareImages(initCmdBuffer, 1, &initTransition);

	vkEndCommandBuffer(initCmdBuffer);

	vkQueueSubmit(commandQueue, 1, &initCmdBuffer, 1, &imageMemoryRef, 0);
}

void createTargetStates(GR_DEVICE device, GR_MSAA_STATE_OBJECT& msaaState, GR_VIEWPORT_STATE_OBJECT& viewportState,
	GR_COLOR_BLEND_STATE_OBJECT& colorBlendState, GR_DEPTH_STENCIL_STATE_OBJECT& depthStencilState, GR_RASTER_STATE_OBJECT& rasterState)
{

	// Create target states
	GR_MSAA_STATE_CREATE_INFO msaaStateCreateInfo = {};
	msaaStateCreateInfo.samples = 1;
	msaaStateCreateInfo.sampleMask = 0xF; // RGBA bits

	vkCreateMsaaState(device, &msaaStateCreateInfo, &msaaState);

	GR_VIEWPORT_STATE_CREATE_INFO viewportStateCreateInfo = {};
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.scissorEnable = GR_FALSE;
	viewportStateCreateInfo.viewports[0].width = WIDTH;
	viewportStateCreateInfo.viewports[0].height = HEIGHT;
	viewportStateCreateInfo.viewports[0].minDepth = 0;
	viewportStateCreateInfo.viewports[0].maxDepth = 1;

	vkCreateViewportState(device, &viewportStateCreateInfo, &viewportState);

	GR_COLOR_BLEND_STATE_CREATE_INFO blendStateCreateInfo = {};
	blendStateCreateInfo.target[0].blendEnable = GR_TRUE;
	blendStateCreateInfo.target[0].srcBlendColor = GR_BLEND_SRC_ALPHA;
	blendStateCreateInfo.target[0].destBlendColor = GR_BLEND_ONE_MINUS_SRC_ALPHA;
	blendStateCreateInfo.target[0].blendFuncColor = GR_BLEND_FUNC_ADD;

	blendStateCreateInfo.target[0].srcBlendAlpha = GR_BLEND_ONE;
	blendStateCreateInfo.target[0].destBlendAlpha = GR_BLEND_ONE;
	blendStateCreateInfo.target[0].blendFuncAlpha = GR_BLEND_FUNC_ADD;

	vkCreateColorBlendState(device, &blendStateCreateInfo, &colorBlendState);

	GR_DEPTH_STENCIL_STATE_CREATE_INFO depthStencilStateCreateInfo = {};
	depthStencilStateCreateInfo.depthEnable = GR_FALSE;
	depthStencilStateCreateInfo.stencilEnable = GR_FALSE;
	depthStencilStateCreateInfo.depthFunc = GR_COMPARE_LESS;

	depthStencilStateCreateInfo.front.stencilDepthFailOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.front.stencilFailOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.front.stencilPassOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.front.stencilFunc = GR_COMPARE_ALWAYS;
	depthStencilStateCreateInfo.front.stencilRef = 0;

	depthStencilStateCreateInfo.back.stencilDepthFailOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.stencilFailOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.stencilPassOp = GR_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.stencilFunc = GR_COMPARE_ALWAYS;
	depthStencilStateCreateInfo.back.stencilRef = 0;

	vkCreateDepthStencilState(device, &depthStencilStateCreateInfo, &depthStencilState);

	GR_RASTER_STATE_CREATE_INFO rasterStateCreateInfo = {};
	rasterStateCreateInfo.fillMode = GR_FILL_SOLID;
	rasterStateCreateInfo.cullMode = GR_CULL_NONE;
	rasterStateCreateInfo.frontFace = GR_FRONT_FACE_CCW;

	vkCreateRasterState(device, &rasterStateCreateInfo, &rasterState);
}

GR_SHADER createShader(GR_DEVICE device, const std::string& filename)
{
	GR_SHADER shader;
	GR_SHADER_CREATE_INFO shaderCreateInfo = {};

	auto vertexShaderCode = loadShader(filename);

	shaderCreateInfo.pCode = vertexShaderCode.data();
	shaderCreateInfo.codeSize = vertexShaderCode.size();

	vkCreateShader(device, &shaderCreateInfo, &shader);

	return shader;
}

void createGraphicsPipeline(GR_DEVICE device, GR_PIPELINE& pipeline, GR_MEMORY_REF& pipelineMemRef)
{
	// Load shaders
	GR_SHADER vertexShader = createShader(device, "assets/shaders/mantle/vs.bin");
	GR_SHADER fragShader = createShader(device, "assets/shaders/mantle/ps.bin");

	// Specify descriptor slots
	GR_DESCRIPTOR_SLOT_INFO vsDescriptorSlots[2];

	vsDescriptorSlots[0].slotObjectType = GR_SLOT_SHADER_RESOURCE;
	vsDescriptorSlots[0].shaderEntityIndex = 0;

	vsDescriptorSlots[1].slotObjectType = GR_SLOT_SHADER_RESOURCE;
	vsDescriptorSlots[1].shaderEntityIndex = 1;

	GR_DESCRIPTOR_SLOT_INFO psDescriptorSlots[2];

	psDescriptorSlots[0].slotObjectType = GR_SLOT_UNUSED;
	psDescriptorSlots[1].slotObjectType = GR_SLOT_UNUSED;

	// Create vkaphics pipeline
	GR_GRAPHICS_PIPELINE_CREATE_INFO pipelineCreateInfo = {};

	pipelineCreateInfo.vs.shader = vertexShader;
	pipelineCreateInfo.vs.dynamicMemoryViewMapping.slotObjectType = GR_SLOT_UNUSED;
	pipelineCreateInfo.vs.descriptorSetMapping[0].descriptorCount = 2;
	pipelineCreateInfo.vs.descriptorSetMapping[0].pDescriptorInfo = vsDescriptorSlots;

	pipelineCreateInfo.ps.shader = fragShader;
	pipelineCreateInfo.ps.dynamicMemoryViewMapping.slotObjectType = GR_SLOT_UNUSED;
	pipelineCreateInfo.ps.descriptorSetMapping[0].descriptorCount = 2;
	pipelineCreateInfo.ps.descriptorSetMapping[0].pDescriptorInfo = psDescriptorSlots;

	pipelineCreateInfo.iaState.topology = GR_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.iaState.disableVertexReuse = GR_FALSE;

	pipelineCreateInfo.rsState.depthClipEnable = GR_FALSE;

	pipelineCreateInfo.cbState.logicOp = GR_LOGIC_OP_COPY;
	pipelineCreateInfo.cbState.target[0].blendEnable = GR_TRUE;
	pipelineCreateInfo.cbState.target[0].channelWriteMask = 0xF; // RGBA bits
	pipelineCreateInfo.cbState.target[0].format.channelFormat = GR_CH_FMT_R8G8B8A8;
	pipelineCreateInfo.cbState.target[0].format.numericFormat = GR_NUM_FMT_UNORM;

	pipelineCreateInfo.dbState.format.channelFormat = GR_CH_FMT_R4G4B4A4;
	pipelineCreateInfo.dbState.format.numericFormat = GR_NUM_FMT_UNDEFINED;

	vkCreateGraphicsPipeline(device, &pipelineCreateInfo, &pipeline);

	pipelineMemRef = allocateObjectMemory(device, pipeline);
}

void initDescriptorSet(GR_DEVICE device, GR_QUEUE commandQueue, GR_DESCRIPTOR_SET& descriptorSet, GR_MEMORY_REF& descriptorMemRef, GR_MEMORY_REF& vertexDataMemRef)
{
	// Create descriptor set for vertex shader inputs
	GR_DESCRIPTOR_SET_CREATE_INFO descriptorCreateInfo = {};
	descriptorCreateInfo.slots = 2;

	vkCreateDescriptorSet(device, &descriptorCreateInfo, &descriptorSet);

	descriptorMemRef = allocateObjectMemory(device, descriptorSet);

	// Store vertex data in GPU memory
	GR_GPU_MEMORY vertexDataMemory = allocateMappableBuffer(device, sizeof(vertices));

	void* bufferPointer;
	vkMapMemory(vertexDataMemory, 0, &bufferPointer);
	memcpy(bufferPointer, vertices, sizeof(vertices));
	vkUnmapMemory(vertexDataMemory);

	vertexDataMemRef = {};
	vertexDataMemRef.mem = vertexDataMemory;

	// Create and submit command buffer that transitions vertex data memory to being shader accessible
	GR_CMD_BUFFER initDataCmdBuffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &initDataCmdBuffer);

	vkBeginCommandBuffer(initDataCmdBuffer, 0);

	GR_MEMORY_STATE_TRANSITION dataTransition = {};
	dataTransition.mem = vertexDataMemory;
	dataTransition.oldState = GR_MEMORY_STATE_DATA_TRANSFER;
	dataTransition.newState = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;
	dataTransition.offset = 0;
	dataTransition.regionSize = sizeof(vertices);

	vkCmdPrepareMemoryRegions(initDataCmdBuffer, 1, &dataTransition);

	vkEndCommandBuffer(initDataCmdBuffer);

	vkQueueSubmit(commandQueue, 1, &initDataCmdBuffer, 1, &vertexDataMemRef, 0);

	// Attach views to the vertex data to the descriptor set
	vkBeginDescriptorSetUpdate(descriptorSet);

	GR_MEMORY_VIEW_ATTACH_INFO memoryViewAttachInfo = {};
	memoryViewAttachInfo.mem = vertexDataMemory;
	memoryViewAttachInfo.offset = 0;
	memoryViewAttachInfo.stride = sizeof(float) * 4;
	memoryViewAttachInfo.range = sizeof(float) * 12;
	memoryViewAttachInfo.format.channelFormat = GR_CH_FMT_R32G32B32A32;
	memoryViewAttachInfo.format.numericFormat = GR_NUM_FMT_FLOAT;
	memoryViewAttachInfo.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;

	vkAttachMemoryViewDescriptors(descriptorSet, 0, 1, &memoryViewAttachInfo);

	memoryViewAttachInfo.offset = sizeof(float) * 12;
	memoryViewAttachInfo.range = sizeof(float) * 12;

	vkAttachMemoryViewDescriptors(descriptorSet, 1, 1, &memoryViewAttachInfo);

	vkEndDescriptorSetUpdate(descriptorSet);
}

GR_CMD_BUFFER createPrepareBuffer(GR_DEVICE device, GR_IMAGE presentableImage, GR_IMAGE_SUBRESOURCE_RANGE imageColorRange)
{
	// Create command buffer that prepares the color target image for rendering
	GR_CMD_BUFFER buffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &buffer);

	vkBeginCommandBuffer(buffer, 0);

	GR_IMAGE_STATE_TRANSITION transition = {};
	transition.image = presentableImage;
	transition.oldState = GR_WSI_WIN_IMAGE_STATE_PRESENT_WINDOWED;
	transition.newState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
	transition.subresourceRange = imageColorRange;

	vkCmdPrepareImages(buffer, 1, &transition);

	vkEndCommandBuffer(buffer);

	return buffer;
}

GR_CMD_BUFFER createClearBuffer(GR_DEVICE device, GR_IMAGE presentableImage, GR_IMAGE_SUBRESOURCE_RANGE imageColorRange)
{
	// Create command buffer that clears the color target to black
	GR_CMD_BUFFER buffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &buffer);

	vkBeginCommandBuffer(buffer, 0);

	GR_IMAGE_STATE_TRANSITION transition = {};
	transition.image = presentableImage;
	transition.oldState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
	transition.newState = GR_IMAGE_STATE_CLEAR;
	transition.subresourceRange = imageColorRange;

	vkCmdPrepareImages(buffer, 1, &transition);

	float clearColor[] = { 0.0, 0.0, 0.0, 1.0 };
	vkCmdClearColorImage(buffer, presentableImage, clearColor, 1, &imageColorRange);

	transition.image = presentableImage;
	transition.oldState = GR_IMAGE_STATE_CLEAR;
	transition.newState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
	transition.subresourceRange = imageColorRange;

	vkCmdPrepareImages(buffer, 1, &transition);

	vkEndCommandBuffer(buffer);

	return buffer;
}

GR_CMD_BUFFER createDrawTriangleBuffer(GR_DEVICE device, GR_COLOR_TARGET_VIEW colorTargetView, GR_DESCRIPTOR_SET descriptorSet, GR_PIPELINE pipeline)
{
	// Create command buffer that renders the triangle
	GR_CMD_BUFFER buffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &buffer);

	vkBeginCommandBuffer(buffer, 0);

	// Bind render target
	GR_COLOR_TARGET_BIND_INFO colorTargetBindInfo;
	colorTargetBindInfo.view = colorTargetView;
	colorTargetBindInfo.colorTargetState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;

	vkCmdBindTargets(buffer, 1, &colorTargetBindInfo, nullptr);

	// Set up dynamic draw state
	vkCmdBindStateObject(buffer, GR_STATE_BIND_MSAA, msaaState);
	vkCmdBindStateObject(buffer, GR_STATE_BIND_VIEWPORT, viewportState);
	vkCmdBindStateObject(buffer, GR_STATE_BIND_COLOR_BLEND, colorBlendState);
	vkCmdBindStateObject(buffer, GR_STATE_BIND_DEPTH_STENCIL, depthStencilState);
	vkCmdBindStateObject(buffer, GR_STATE_BIND_RASTER, rasterState);

	// Bind descriptor set
	vkCmdBindDescriptorSet(buffer, GR_PIPELINE_BIND_POINT_GRAPHICS, 0, descriptorSet, 0);

	// Set vkaphics pipeline
	vkCmdBindPipeline(buffer, GR_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	// Render triangle
	vkCmdDraw(buffer, 0, 3, 0, 1);

	vkEndCommandBuffer(buffer);

	return buffer;
}

GR_CMD_BUFFER createFinishBuffer(GR_DEVICE device, GR_IMAGE presentableImage, GR_IMAGE_SUBRESOURCE_RANGE imageColorRange)
{
	// Create command buffer that transitions color target image back to a presentable state
	GR_CMD_BUFFER buffer;
	vkCreateCommandBuffer(device, &bufferCreateInfo, &buffer);

	vkBeginCommandBuffer(buffer, 0);

	GR_IMAGE_STATE_TRANSITION transition = {};
	transition.image = presentableImage;
	transition.oldState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
	transition.newState = GR_WSI_WIN_IMAGE_STATE_PRESENT_WINDOWED;
	transition.subresourceRange = imageColorRange;

	vkCmdPrepareImages(buffer, 1, &transition);

	vkEndCommandBuffer(buffer);

	return buffer;
}

#include "Utils/DeltaTimeMeasurer.h"
#include "Utils/FPSMeasurer.h"

void testMantle2()
{
	// Initialize function pointers, much like GLEW in OpenGL
	mantleLoadFunctions();

	// Set debug callback
	vkDbgRegisterMsgCallback(debugCallback, nullptr);

	// Create device and presentable image
	GR_DEVICE device;
	GR_QUEUE universalQueue;
	createDeviceAndQueue(device, universalQueue);

	GR_IMAGE presentableImage;
	GR_MEMORY_REF presentableImageMemRef;
	GR_IMAGE_SUBRESOURCE_RANGE imageColorRange;
	initPresentableImage(device, universalQueue, presentableImage, presentableImageMemRef, imageColorRange);

	// Create window to present to
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mantle Hello Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

	GR_WSI_WIN_PRESENT_INFO presentInfo = {};
	presentInfo.hWndDest = GetActiveWindow();
	presentInfo.srcImage = presentableImage;
	presentInfo.presentMode = GR_WSI_WIN_PRESENT_MODE_WINDOWED;

	// Create color target view
	GR_COLOR_TARGET_VIEW colorTargetView;
	GR_COLOR_TARGET_VIEW_CREATE_INFO colorTargetViewCreateInfo = {};
	colorTargetViewCreateInfo.image = presentableImage;
	colorTargetViewCreateInfo.arraySize = 1;
	colorTargetViewCreateInfo.baseArraySlice = 0;
	colorTargetViewCreateInfo.mipLevel = 0;
	colorTargetViewCreateInfo.format.channelFormat = GR_CH_FMT_R8G8B8A8;
	colorTargetViewCreateInfo.format.numericFormat = GR_NUM_FMT_UNORM;

	vkCreateColorTargetView(device, &colorTargetViewCreateInfo, &colorTargetView);

	// Create dynamic states (viewport, depth/stencil operations, etc.)
	createTargetStates(device, msaaState, viewportState, colorBlendState, depthStencilState, rasterState);

	// Create vkaphics pipeline with shaders and descriptor bindings
	GR_PIPELINE pipeline;
	GR_MEMORY_REF pipelineMemRef;
	createGraphicsPipeline(device, pipeline, pipelineMemRef);

	// Set up descriptor set with vertex data memory views
	GR_DESCRIPTOR_SET descriptorSet;
	GR_MEMORY_REF descriptorMemRef, vertexDataMemRef;
	initDescriptorSet(device, universalQueue, descriptorSet, descriptorMemRef, vertexDataMemRef);

	// Create command buffers for rendering steps
	GR_CMD_BUFFER bufferPrepareRender = createPrepareBuffer(device, presentableImage, imageColorRange);
	GR_CMD_BUFFER bufferClear = createClearBuffer(device, presentableImage, imageColorRange);
	GR_CMD_BUFFER bufferDrawTriangle = createDrawTriangleBuffer(device, colorTargetView, descriptorSet, pipeline);
	GR_CMD_BUFFER bufferFinish = createFinishBuffer(device, presentableImage, imageColorRange);

	// Create fence for CPU synchronization with rendering
	GR_FENCE fence;
	GR_FENCE_CREATE_INFO fenceCreateInfo = {};
	vkCreateFence(device, &fenceCreateInfo, &fence);

	DeltaTimeMeasurer deltaTimeMeasurer;
	FPSMeasurer fpsMeasurer;

	fpsMeasurer.setLogFunction(1.0f, [&]() {
		printf("FPS: %i \n", fpsMeasurer.getAvgFps());
	});

	// Main loop
	while (true)
	{
		SDL_Event windowEvent;
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
		}

		fpsMeasurer.tickFrame(deltaTimeMeasurer.calcDeltaSec(SDL_GetTicks()));

		// Wait for previous frame to end
		vkWaitForFences(device, 1, &fence, true, 1);

		// Submit command buffers along with memory references
		GR_MEMORY_REF memoryRefs[] = { presentableImageMemRef, pipelineMemRef, descriptorMemRef, vertexDataMemRef };
		GR_CMD_BUFFER commandBuffers[] = { bufferPrepareRender, bufferClear, bufferDrawTriangle, bufferFinish };

		vkQueueSubmit(universalQueue, 4, commandBuffers, 4, memoryRefs, fence);

		// Present image to the window
		vkWsiWinQueuePresent(universalQueue, &presentInfo);
	}
}