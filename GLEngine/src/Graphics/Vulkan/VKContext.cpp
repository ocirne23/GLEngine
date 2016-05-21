#include "Graphics/Vulkan/VKContext.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKDebug.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/Wrappers/VKBuffer.h"
#include "Graphics/Vulkan/Wrappers/VKPipelineDescription.h"


#include "Utils/DeltaTimeMeasurer.h"
#include "Utils/FPSMeasurer.h"

#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define VERTEX_BUFFER_BIND_ID 0

vk::Instance VKContext::instance = NULL;

vk::PhysicalDevice VKContext::physDevice = NULL;
vk::PhysicalDeviceProperties VKContext::physDeviceProperties;
vk::PhysicalDeviceMemoryProperties VKContext::physDeviceMemoryProperties;
vk::PhysicalDeviceFeatures VKContext::physDeviceFeatures;

vk::Device VKContext::device = NULL;
vk::Queue VKContext::queue = NULL;
vk::CommandPool VKContext::commandPool = NULL;
vk::Format VKContext::depthFormat = vk::Format::eUndefined;
vk::Format VKContext::colorFormat = vk::Format::eUndefined;
vk::ColorSpaceKHR VKContext::colorSpace;

vk::SurfaceKHR VKContext::surface = NULL;
vk::SwapchainKHR VKContext::swapchain = NULL;
eastl::vector<vk::Image> VKContext::swapchainImages;
eastl::vector<vk::ImageView> VKContext::swapchainImageViews;

vk::Semaphore VKContext::presentCompleteSemaphore = NULL;
vk::Semaphore VKContext::renderCompleteSemaphore = NULL;
vk::Semaphore VKContext::textOverlayCompleteSemaphore = NULL;

vk::SubmitInfo VKContext::submitInfo;

eastl::vector<vk::CommandBuffer> VKContext::drawCommandBuffers;
vk::CommandBuffer VKContext::prePresentCommandBuffer = NULL;
vk::CommandBuffer VKContext::postPresentCommandBuffer = NULL;

vk::Image VKContext::depthStencilImage = NULL;
vk::DeviceMemory VKContext::depthStencilMemory = NULL;
vk::ImageView VKContext::depthStencilImageView = NULL;

vk::PipelineCache VKContext::pipelineCache = NULL;

vk::RenderPass VKContext::renderPass = NULL;
eastl::vector<vk::Framebuffer> VKContext::framebuffers;
uint VKContext::currentBuffer = 0;

vk::PipelineLayout VKContext::pipelineLayout;

vk::Pipeline VKContext::pipeline = NULL;
vk::DescriptorPool VKContext::descriptorPool = NULL;
vk::DescriptorSet VKContext::descriptorSet = NULL;

void VKContext::initialize()
{
	uint windowWidth = GLEngine::graphics->getWindowWidth();
	uint windowHeight = GLEngine::graphics->getWindowHeight();

	/* Create instance */
	eastl::vector<const char*> instanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};
	if (VKDebug::VALIDATION_ENABLED)
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.setApiVersion(VK_API_VERSION_1_0)
		.setEngineVersion(0)
		.setPEngineName("GLEngine")
		.setApplicationVersion(0)
		.setPApplicationName("GLApp");
	vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
		.setPApplicationInfo(&appInfo)
		.setEnabledExtensionCount(uint(instanceExtensions.size()))
		.setPpEnabledExtensionNames(instanceExtensions.data());
	if (VKDebug::VALIDATION_ENABLED)
	{
		instanceCreateInfo.setEnabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT);
		instanceCreateInfo.setPpEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	instance = vk::createInstance(instanceCreateInfo);

	/* Get physdevice */
	physDevice = instance.enumeratePhysicalDevices()[0];
	eastl::vector<vk::QueueFamilyProperties> queueFamilyProperties = physDevice.getQueueFamilyProperties();
	uint graphicsQueueIdx;
	for (graphicsQueueIdx = 0; graphicsQueueIdx < queueFamilyProperties.size(); ++graphicsQueueIdx)
		if (queueFamilyProperties[graphicsQueueIdx].queueFlags & vk::QueueFlagBits::eGraphics)
			break;
	assert(graphicsQueueIdx < queueFamilyProperties.size());

	physDeviceProperties = physDevice.getProperties();
	physDeviceMemoryProperties = physDevice.getMemoryProperties();
	physDeviceFeatures = physDevice.getFeatures();

	/* Create device */
	eastl::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	eastl::array<float, 1> queuePriorities = { 0.0f };
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
		.setQueueFamilyIndex(graphicsQueueIdx)
		.setQueueCount(1)
		.setPQueuePriorities(queuePriorities.data());
	vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&deviceQueueCreateInfo)
		.setEnabledExtensionCount(uint(deviceExtensions.size()))
		.setPpEnabledExtensionNames(deviceExtensions.data());

	if (VKDebug::VALIDATION_ENABLED)
	{
		deviceCreateInfo.setEnabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT);
		deviceCreateInfo.setPpEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}
	device = physDevice.createDevice(deviceCreateInfo);

	/* Get queue */
	queue = device.getQueue(graphicsQueueIdx, 0);

	/* Find valid depth format */
	eastl::vector<vk::Format> depthFormats = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD16Unorm
	};
	depthFormat = vk::Format::eUndefined;
	for (vk::Format format : depthFormats)
	{
		vk::FormatProperties formatProperties = physDevice.getFormatProperties(format);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			depthFormat = format;
			break;
		}
	}
	assert(depthFormat != vk::Format::eUndefined);

	/* Create semaphores */
	presentCompleteSemaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	renderCompleteSemaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	textOverlayCompleteSemaphore = device.createSemaphore(vk::SemaphoreCreateInfo());

	/* Setup submitinfo */
	vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eBottomOfPipe;
	submitInfo = vk::SubmitInfo()
		.setPWaitDstStageMask(&submitPipelineStages)
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&presentCompleteSemaphore)
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&renderCompleteSemaphore);

	/* Init surface */
	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.setHwnd(scast<HWND>(GLEngine::graphics->getHWND()));
	surface = instance.createWin32SurfaceKHR(win32SurfaceCreateInfo);
	vk::Bool32 supportsKHR = physDevice.getSurfaceSupportKHR(graphicsQueueIdx, surface);
	assert(supportsKHR);

	/* Get color format/space */
	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats = physDevice.getSurfaceFormatsKHR(surface);
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
	{
		colorFormat = vk::Format::eB8G8R8A8Unorm;
	}
	else
	{
		assert(surfaceFormats.size() >= 1);
		colorFormat = surfaceFormats[0].format;
	}
	colorSpace = surfaceFormats[0].colorSpace;

	/* Setup debugging */
	if (VKDebug::VALIDATION_ENABLED)
		VKDebug::setupDebugging(instance, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, NULL);

	/* Create commandpool */
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(graphicsQueueIdx)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	commandPool = device.createCommandPool(commandPoolCreateInfo);

	/* Create&begin setup commandbuffer */
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(commandPool);
	vk::CommandBuffer setupCommandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];
	setupCommandBuffer.begin(vk::CommandBufferBeginInfo());

	/* Setup swapchain */
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	physDevice.getSurfaceCapabilitiesKHR(surface, surfaceCapabilities);

	eastl::vector<vk::PresentModeKHR> presentModes = physDevice.getSurfacePresentModesKHR(surface);

	vk::Extent2D swapchainExtent(windowWidth, windowHeight);
	if (surfaceCapabilities.currentExtent.width != -1)
	{
		swapchainExtent = surfaceCapabilities.currentExtent;
		// TODO: handle resolution change
		assert(windowWidth == swapchainExtent.width && windowHeight == swapchainExtent.height);
	}

	vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
	for (uint i = 0; i < presentModes.size(); ++i)
	{
		if (presentModes[i] == vk::PresentModeKHR::eMailbox)
		{
			swapchainPresentMode = vk::PresentModeKHR::eMailbox;
			break;
		}
		if ((swapchainPresentMode != vk::PresentModeKHR::eMailbox) && presentModes[i] == vk::PresentModeKHR::eImmediate)
			swapchainPresentMode = vk::PresentModeKHR::eImmediate;
	}

	uint desiredNumSwapchainImages = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumSwapchainImages > surfaceCapabilities.maxImageCount))
		desiredNumSwapchainImages = surfaceCapabilities.maxImageCount;

	vk::SurfaceTransformFlagBitsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	else
		preTransform = surfaceCapabilities.currentTransform;

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
		.setSurface(surface)
		.setMinImageCount(desiredNumSwapchainImages)
		.setImageFormat(colorFormat)
		.setImageColorSpace(colorSpace)
		.setImageExtent(swapchainExtent)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(preTransform)
		.setImageArrayLayers(1)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndexCount(0)
		.setPQueueFamilyIndices(NULL)
		.setPresentMode(swapchainPresentMode)
		.setOldSwapchain(NULL)
		.setClipped(VK_TRUE)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapchain = device.createSwapchainKHR(swapchainCreateInfo);

	swapchainImages = device.getSwapchainImagesKHR(swapchain);
	swapchainImageViews.resize(swapchainImages.size());

	for (uint i = 0; i < swapchainImages.size(); ++i)
	{
		VKUtils::setImageLayout(setupCommandBuffer, swapchainImages[i], vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

		vk::ImageViewCreateInfo colorAttachmentViewCreateInfo = vk::ImageViewCreateInfo()
			.setFormat(colorFormat)
			.setComponents(vk::ComponentMapping()
				.setR(vk::ComponentSwizzle::eR)
				.setG(vk::ComponentSwizzle::eG)
				.setB(vk::ComponentSwizzle::eB)
				.setA(vk::ComponentSwizzle::eA))
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1))
			.setViewType(vk::ImageViewType::e2D)
			.setImage(swapchainImages[i]);
		swapchainImageViews[i] = device.createImageView(colorAttachmentViewCreateInfo, NULL);
	}

	/* Create commandbuffers */
	commandBufferAllocateInfo.setCommandBufferCount(uint(swapchainImages.size()));
	drawCommandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);
	commandBufferAllocateInfo.setCommandBufferCount(1);
	prePresentCommandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];
	postPresentCommandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

	/* Setup depthstencil */
	vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(depthFormat)
		.setExtent(vk::Extent3D(windowWidth, windowHeight, 1))
		.setMipLevels(1)
		.setArrayLayers(1)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setTiling(vk::ImageTiling::eOptimal)
		.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);
	depthStencilImage = device.createImage(imageCreateInfo);

	vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(depthStencilImage);
	uint memoryTypeIndex = VKUtils::getMemoryType(physDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(memoryRequirements.size)
		.setMemoryTypeIndex(memoryTypeIndex);
	depthStencilMemory = device.allocateMemory(memoryAllocateInfo);

	device.bindImageMemory(depthStencilImage, depthStencilMemory, 0);

	VKUtils::setImageLayout(setupCommandBuffer, depthStencilImage, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::ImageViewCreateInfo depthStencilViewCreateInfo = vk::ImageViewCreateInfo()
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(depthFormat)
		.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1))
		.setImage(depthStencilImage);
	depthStencilImageView = device.createImageView(depthStencilViewCreateInfo, NULL);

	/* Setup renderpass */
	vk::AttachmentDescription attachments[2] = {
		vk::AttachmentDescription()
			.setFormat(colorFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal),
		vk::AttachmentDescription()
			.setFormat(depthFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
	};
	vk::AttachmentReference colorReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthReference = vk::AttachmentReference()
		.setAttachment(1)
		.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
	vk::SubpassDescription subpass = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setFlags(vk::SubpassDescriptionFlagBits())
		.setInputAttachmentCount(0)
		.setPInputAttachments(NULL)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorReference)
		.setPResolveAttachments(NULL)
		.setPDepthStencilAttachment(&depthReference)
		.setPreserveAttachmentCount(0)
		.setPPreserveAttachments(NULL);
	vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
		.setPNext(NULL)
		.setAttachmentCount(2)
		.setPAttachments(attachments)
		.setSubpassCount(1)
		.setPSubpasses(&subpass)
		.setDependencyCount(0)
		.setPDependencies(NULL);
	renderPass = device.createRenderPass(renderPassInfo, NULL);

	/* Create pipelinecache */
	pipelineCache = device.createPipelineCache(vk::PipelineCacheCreateInfo());

	/* Setup framebuffer */
	vk::ImageView framebufferAttachments[2];
	framebufferAttachments[1] = depthStencilImageView;

	framebuffers.resize(swapchainImages.size());
	for (uint i = 0; i < framebuffers.size(); ++i)
	{
		framebufferAttachments[0] = swapchainImageViews[i];
		vk::FramebufferCreateInfo framebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass)
			.setAttachmentCount(uint(ARRAY_SIZE(framebufferAttachments)))
			.setPAttachments(&framebufferAttachments[0])
			.setWidth(windowWidth)
			.setHeight(windowHeight)
			.setLayers(1);
		framebuffers[i] = device.createFramebuffer(framebufferCreateInfo);
	}

	/* Flush setup commandbuffer*/
	setupCommandBuffer.end();

	vk::SubmitInfo submitInfo = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&setupCommandBuffer);
	queue.submit(submitInfo, NULL);
	queue.waitIdle();
}

void VKContext::test()
{
	initialize();

	AssetDatabase db;
	db.openExisting("assets/OBJ-DB.da");
	DBScene* scene = dcast<DBScene*>(db.loadAsset("palace.obj", EAssetType::SCENE));
	scene->mergeMeshes();
	DBMesh& mesh = scene->getMeshes()[0];

	/* Create vertex & indice buffers */
	eastl::vector<DBMesh::Vertex> vertexData = mesh.getVertices();
	eastl::vector<uint> indexData = mesh.getIndices();
	uint64 vertexBufferSize = vertexData.size_bytes();
	uint64 indexBufferSize = indexData.size_bytes();

	// Vertex buffer
	VKBuffer vertexStagingBuffer;
	vertexStagingBuffer.initialize(vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible);
	
	void* vertexStagingBufferMapping = vertexStagingBuffer.mapMemory();
	memcpy(vertexStagingBufferMapping, vertexData.data(), vertexBufferSize);
	vertexStagingBuffer.unmapMemory();

	VKBuffer vertexDestinationBuffer;
	vertexDestinationBuffer.initialize(vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	// Index buffer
	VKBuffer indexStagingBuffer;
	indexStagingBuffer.initialize(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible);

	void* indexStagingBufferMapping = indexStagingBuffer.mapMemory();
	memcpy(indexStagingBufferMapping, indexData.data(), indexBufferSize);
	indexStagingBuffer.unmapMemory();

	VKBuffer indexDestinationBuffer;
	indexDestinationBuffer.initialize(indexBufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	// Copy staging buffers to destination buffers
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(commandPool);
	vk::CommandBuffer copyCommandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];
	copyCommandBuffer.begin(vk::CommandBufferBeginInfo());

	vk::BufferCopy copyRegion = vk::BufferCopy()
		.setSize(vertexBufferSize);
	copyCommandBuffer.copyBuffer(vertexStagingBuffer, vertexDestinationBuffer, 1, &copyRegion);
	copyRegion.setSize(indexBufferSize);
	copyCommandBuffer.copyBuffer(indexStagingBuffer, indexDestinationBuffer, 1, &copyRegion);

	copyCommandBuffer.end();
	vk::SubmitInfo copyCommandBufferSubmitInfo = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&copyCommandBuffer);
	queue.submit(copyCommandBufferSubmitInfo, NULL);
	queue.waitIdle();

	device.freeCommandBuffers(commandPool, copyCommandBuffer);
	vertexStagingBuffer.cleanup();
	indexStagingBuffer.cleanup();

	/* Prepare uniform buffer */
	struct UniformData {
		glm::mat4 mvpMatrix;
	} uniformData;

	uint windowWidth = GLEngine::graphics->getWindowWidth();
	uint windowHeight = GLEngine::graphics->getWindowHeight();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), float(windowWidth) / float(windowHeight), 0.1f, 256.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 8.0f, -16.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0, -1, 0));
	glm::mat4 modelMatrix = glm::mat4();
	uniformData.mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

	VKBuffer uniformBuffer;
	{
		uniformBuffer.initialize(sizeof(uniformData), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible);
		void* uniformBufferMapping = uniformBuffer.mapMemory();
		memcpy(uniformBufferMapping, &uniformData, sizeof(uniformData));
		uniformBuffer.unmapMemory();
	}

	/////
	VKPipelineDescription pipelineDescription;

	pipelineDescription.bindingDescriptions = {
		vk::VertexInputBindingDescription()
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setStride(sizeof(vertexData[0]))
			.setInputRate(vk::VertexInputRate::eVertex)
	};
	pipelineDescription.attributeDescriptions = {
		vk::VertexInputAttributeDescription() // Position
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(0),
		vk::VertexInputAttributeDescription() // Texcoord
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(1)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setOffset(sizeof(glm::vec3)),
		vk::VertexInputAttributeDescription() // Normal
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(2)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(sizeof(glm::vec3) + sizeof(glm::vec2)),
		vk::VertexInputAttributeDescription() // MaterialID
			.setBinding(VERTEX_BUFFER_BIND_ID)
			.setLocation(3)
			.setFormat(vk::Format::eR32Uint)
			.setOffset(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3))
	};
	pipelineDescription.descriptors = 
	{	// buffer, type, stage, size, offset, binding
		{ &uniformBuffer, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, sizeof(uniformData), 0, 0 }, // u_mvpMatrix UBO
	};

	setupPipeline(pipelineDescription);
	buildCommandBuffers(vertexDestinationBuffer, indexDestinationBuffer, uint(indexData.size()), vk::IndexType::eUint32);

	FPSMeasurer fpsMeasurer;
	DeltaTimeMeasurer timer;
	fpsMeasurer.setLogFunction(5.0f, [&]() {
		print("FPS: %i, MS:%f\n", fpsMeasurer.getAvgFps(), fpsMeasurer.getAvgMsPerFrame());
	});

	/* Render loop */
	while (!GLEngine::isShutdown())
	{
		render();
		fpsMeasurer.tickFrame(timer.calcDeltaSec(GLEngine::getTimeMs()));
	}
}

void VKContext::buildCommandBuffers(VKBuffer& vertexBuffer, VKBuffer& indexBuffer, uint numIndices, vk::IndexType indexType)
{
	uint windowWidth = GLEngine::graphics->getWindowWidth();
	uint windowHeight = GLEngine::graphics->getWindowHeight();

	/* Build command buffers */
	vk::ClearValue clearValues[2] = {
		vk::ClearColorValue(eastl::array<float, 4>{0.025f, 0.025f, 0.025f, 1.0f}),
		vk::ClearDepthStencilValue(1.0f, 0)
	};

	vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
		.setRenderPass(renderPass)
		.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(windowWidth, windowHeight)))
		.setClearValueCount(2)
		.setPClearValues(clearValues);

	vk::Viewport viewport = vk::Viewport()
		.setWidth(float(windowWidth))
		.setHeight(float(windowHeight))
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	vk::Rect2D scissor = vk::Rect2D()
		.setExtent(vk::Extent2D(windowWidth, windowHeight))
		.setOffset(vk::Offset2D(0, 0));

	for (int32_t i = 0; i < drawCommandBuffers.size(); ++i)
	{
		renderPassBeginInfo.setFramebuffer(framebuffers[i]);
		vk::CommandBuffer commandBuffer = drawCommandBuffers[i];
		commandBuffer.begin(vk::CommandBufferBeginInfo());
		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
		commandBuffer.setViewport(0, viewport);
		commandBuffer.setScissor(0, scissor);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, { descriptorSet }, {});
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		commandBuffer.bindVertexBuffers(VERTEX_BUFFER_BIND_ID, { vertexBuffer }, vk::DeviceSize(0));
		commandBuffer.bindIndexBuffer(indexBuffer, vk::DeviceSize(0), indexType);
		commandBuffer.drawIndexed(numIndices, 1, 0, 0, 1);
		commandBuffer.endRenderPass();

		vk::ImageMemoryBarrier prePresentBarrier = vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setDstAccessMask(vk::AccessFlags())
			.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1))
			.setImage(swapchainImages[i]);

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlags(),
			eastl::vector<vk::MemoryBarrier>{},
			eastl::vector<vk::BufferMemoryBarrier>{},
			eastl::vector<vk::ImageMemoryBarrier>{prePresentBarrier});
		commandBuffer.end();
	}
}

void VKContext::setupPipeline(VKPipelineDescription& pipelineDescription)
{
	/* Setup descriptor set layout */
	eastl::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
	eastl::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	for (auto& descriptor : pipelineDescription.descriptors)
	{
		descriptorBufferInfos.push_back(vk::DescriptorBufferInfo()
			.setBuffer(*descriptor.buffer)
			.setOffset(descriptor.offset)
			.setRange(descriptor.size));
		vk::DescriptorSetLayoutBinding layoutBinding = vk::DescriptorSetLayoutBinding()
			.setDescriptorType(descriptor.type)
			.setDescriptorCount(1)
			.setStageFlags(descriptor.stage)
			.setBinding(descriptor.binding);
		descriptorSetLayoutBindings.push_back(layoutBinding);
	}
	vk::DescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(uint(descriptorSetLayoutBindings.size()))
		.setPBindings(descriptorSetLayoutBindings.data());
	vk::DescriptorSetLayout descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayoutCreateInfo, NULL);

	vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount(1)
		.setPSetLayouts(&descriptorSetLayout);
	pipelineLayout = device.createPipelineLayout(pPipelineLayoutCreateInfo, NULL);

	/* Setup descriptor pool */
	eastl::vector<vk::DescriptorPoolSize> typeCounts;
	auto makeDescriptorPoolType = [&](vk::DescriptorType a_type)
	{
		uint numDescriptorsWithType = std::accumulate(pipelineDescription.descriptors.begin(), pipelineDescription.descriptors.end(), 0, [&](uint a_accum, VKPipelineDescription::Descriptor& a_item)
		{
			return a_accum + (a_type == a_item.type ? 1 : 0);
		});
		if (numDescriptorsWithType) typeCounts.push_back(vk::DescriptorPoolSize().setType(a_type).setDescriptorCount(numDescriptorsWithType));
	};
	for (uint i = uint(vk::DescriptorType::eSampler); i != uint(vk::DescriptorType::eInputAttachment); ++i)
		makeDescriptorPoolType(vk::DescriptorType(i));

	vk::DescriptorPoolCreateInfo descriptorPoolInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizeCount(uint(typeCounts.size()))
		.setPPoolSizes(typeCounts.data())
		// Count the total number of descriptors
		.setMaxSets(std::accumulate(typeCounts.begin(), typeCounts.end(), 0, [](uint accum, vk::DescriptorPoolSize& item) { return accum + item.descriptorCount; }));
	descriptorPool = device.createDescriptorPool(descriptorPoolInfo, NULL);

	/* Setup descriptor set */
	vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(uint(1))
		.setPSetLayouts(&descriptorSetLayout);
	descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

	eastl::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	for (uint i = 0; i < uint(descriptorBufferInfos.size()); ++i)
	{
		writeDescriptorSets.push_back(vk::WriteDescriptorSet()
			.setDescriptorCount(1)
			.setDstSet(descriptorSet)
			.setDescriptorType(descriptorSetLayoutBindings[i].descriptorType)
			.setPBufferInfo(&descriptorBufferInfos[i])
			.setDstBinding(descriptorSetLayoutBindings[i].binding));
	}
	device.updateDescriptorSets(writeDescriptorSets, {});

	/* Prepare pipelines */
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(pipelineDescription.topology);

	vk::PipelineRasterizationStateCreateInfo rasterizationState = vk::PipelineRasterizationStateCreateInfo()
		.setPolygonMode(pipelineDescription.polygonMode)
		.setCullMode(pipelineDescription.cullFace)
		.setFrontFace(vk::FrontFace::eClockwise)
		.setDepthClampEnable(VK_FALSE)
		.setRasterizerDiscardEnable(VK_FALSE)
		.setDepthBiasEnable(VK_FALSE)
		.setLineWidth(pipelineDescription.lineWidth);

	vk::PipelineColorBlendAttachmentState blendAttachmentState[1] = {};
	blendAttachmentState[0] = vk::PipelineColorBlendAttachmentState()
		.setColorWriteMask(pipelineDescription.colorWriteMask)
		.setBlendEnable(pipelineDescription.blendingEnabled);

	vk::PipelineColorBlendStateCreateInfo colorBlendState = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(1)
		.setPAttachments(blendAttachmentState);

	vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setScissorCount(1);

	vk::DynamicState dynamicStates[] = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	vk::PipelineDynamicStateCreateInfo dynamicState = vk::PipelineDynamicStateCreateInfo()
		.setDynamicStateCount(ARRAY_SIZE(dynamicStates))
		.setPDynamicStates(dynamicStates);

	vk::PipelineDepthStencilStateCreateInfo depthStencilState = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(pipelineDescription.depthTestEnabled)
		.setDepthWriteEnable(pipelineDescription.depthWriteEnabled)
		.setDepthCompareOp(pipelineDescription.depthCompareOp)
		.setDepthBoundsTestEnable(pipelineDescription.depthBoundTestEnabled)
		.setBack(vk::StencilOpState()
			.setFailOp(vk::StencilOp::eKeep)
			.setPassOp(vk::StencilOp::eKeep)
			.setCompareOp(vk::CompareOp::eAlways))
		.setStencilTestEnable(pipelineDescription.stencilTestEnabled)
		.setFront(vk::StencilOpState()
			.setFailOp(vk::StencilOp::eKeep)
			.setPassOp(vk::StencilOp::eKeep)
			.setCompareOp(vk::CompareOp::eAlways));

	vk::PipelineMultisampleStateCreateInfo multisampleState = vk::PipelineMultisampleStateCreateInfo()
		.setRasterizationSamples(pipelineDescription.multisamples);

	vk::PipelineShaderStageCreateInfo shaderStages[2] = { {},{} };
	shaderStages[0] = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(VKUtils::loadShaderModule(pipelineDescription.vertexShaderPath.c_str(), device))
		.setPName("main");
	shaderStages[1] = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(VKUtils::loadShaderModule(pipelineDescription.fragmentShaderPath.c_str(), device))
		.setPName("main");

	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo()
		.setVertexBindingDescriptionCount(uint(pipelineDescription.bindingDescriptions.size()))
		.setPVertexBindingDescriptions(pipelineDescription.bindingDescriptions.data())
		.setVertexAttributeDescriptionCount(uint(pipelineDescription.attributeDescriptions.size()))
		.setPVertexAttributeDescriptions(pipelineDescription.attributeDescriptions.data());

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setLayout(pipelineLayout)
		.setPVertexInputState(&vertexInputStateCreateInfo)
		.setPInputAssemblyState(&inputAssemblyState)
		.setPRasterizationState(&rasterizationState)
		.setPColorBlendState(&colorBlendState)
		.setPMultisampleState(&multisampleState)
		.setPViewportState(&viewportState)
		.setPDepthStencilState(&depthStencilState)
		.setStageCount(ARRAY_SIZE(shaderStages))
		.setPStages(shaderStages)
		.setRenderPass(renderPass)
		.setPDynamicState(&dynamicState);
	pipeline = device.createGraphicsPipelines(pipelineCache, { pipelineCreateInfo }, NULL)[0];
}

void VKContext::render()
{
	device.waitIdle();

	device.acquireNextImageKHR(swapchain, UINT64_MAX, presentCompleteSemaphore, NULL, currentBuffer);

	vk::ImageMemoryBarrier postPresentBarrier = vk::ImageMemoryBarrier()
		.setSrcAccessMask(vk::AccessFlags())
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
		.setOldLayout(vk::ImageLayout::ePresentSrcKHR)
		.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1))
		.setImage(swapchainImages[currentBuffer]);

	postPresentCommandBuffer.begin(vk::CommandBufferBeginInfo());
	postPresentCommandBuffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eAllCommands,
		vk::PipelineStageFlagBits::eTopOfPipe,
		vk::DependencyFlags(),
		eastl::vector<vk::MemoryBarrier>{},
		eastl::vector<vk::BufferMemoryBarrier>{},
		eastl::vector<vk::ImageMemoryBarrier>{postPresentBarrier});
	postPresentCommandBuffer.end();

	vk::SubmitInfo postPresentCommandBufferSubmitInfo = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&postPresentCommandBuffer);
	queue.submit(postPresentCommandBufferSubmitInfo, VK_NULL_HANDLE);
	queue.waitIdle();

	vk::PipelineStageFlags pipelineStages = vk::PipelineStageFlagBits::eBottomOfPipe;
	vk::SubmitInfo drawCommandBufferSubmitInfo = vk::SubmitInfo()
		.setPWaitDstStageMask(&pipelineStages)
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&presentCompleteSemaphore)
		.setCommandBufferCount(1)
		.setPCommandBuffers(&drawCommandBuffers[currentBuffer])
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&renderCompleteSemaphore);
	queue.submit(drawCommandBufferSubmitInfo, VK_NULL_HANDLE);
	queue.waitIdle();

	vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
		.setSwapchainCount(1)
		.setPSwapchains(&swapchain)
		.setPImageIndices(&currentBuffer)
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&renderCompleteSemaphore);
	queue.presentKHR(presentInfo);

	device.waitIdle();
}
