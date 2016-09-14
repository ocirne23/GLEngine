/*
* Vulkan Example base class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include "Graphics/Vulkan/Examples/vulkanExampleBase.h"

#include "GLEngine.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "Utils/DeltaTimeMeasurer.h"
#include "Utils/FPSMeasurer.h"
#include "Utils/StringUtils.h"

using namespace vkx;

ExampleBase::ExampleBase(bool enableValidation) : swapChain(*this) {
	// Check for validation command line flag
#if defined(_WIN32)
	for (int32_t i = 0; i < __argc; i++) {
		if (__argv[i] == std::string("-validation")) {
			enableValidation = true;
		}
	}
#elif defined(__ANDROID__)
	// Vulkan library is loaded dynamically on Android
	bool libLoaded = loadVulkanLibrary();
	assert(libLoaded);
#endif

#if !defined(__ANDROID__)
	// Android Vulkan initialization is handled in APP_CMD_INIT_WINDOW event
	initVulkan(enableValidation);
#endif
}

ExampleBase::~ExampleBase() {
	// Clean up Vulkan resources
	swapChain.cleanup();
	if (descriptorPool) {
		device.destroyDescriptorPool(descriptorPool);
	}
	if (!primaryCmdBuffers.empty()) {
		device.freeCommandBuffers(cmdPool, primaryCmdBuffers);
		primaryCmdBuffers.clear();
	}
	if (!drawCmdBuffers.empty()) {
		device.freeCommandBuffers(cmdPool, drawCmdBuffers);
		drawCmdBuffers.clear();
	}
	if (!textCmdBuffers.empty()) {
		device.freeCommandBuffers(cmdPool, textCmdBuffers);
		textCmdBuffers.clear();
	}
	device.destroyRenderPass(renderPass);
	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		device.destroyFramebuffer(framebuffers[i]);
	}

	for (auto& shaderModule : shaderModules) {
		device.destroyShaderModule(shaderModule);
	}
	depthStencil.destroy();

	if (textureLoader) {
		delete textureLoader;
	}

	if (enableTextOverlay) {
		delete textOverlay;
	}

	device.destroySemaphore(semaphores.acquireComplete);
	device.destroySemaphore(semaphores.renderComplete);

	destroyContext();
}


void ExampleBase::run() {
	setupWindow();
	prepare();

	renderLoop();

	queue.waitIdle();
	device.waitIdle();
}

void ExampleBase::initVulkan(bool enableValidation) {
	createContext(enableValidation);
	// Find a suitable depth format
	depthFormat = getSupportedDepthFormat(physicalDevice);

	// Create synchronization objects
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	// Create a semaphore used to synchronize image presentation
	// Ensures that the image is displayed before we start submitting new commands to the queu
	semaphores.acquireComplete = device.createSemaphore(semaphoreCreateInfo);
	// Create a semaphore used to synchronize command submission
	// Ensures that the image is not presented until all commands have been sumbitted and executed
	semaphores.renderComplete = device.createSemaphore(semaphoreCreateInfo);

	// Set up submit info structure
	// Semaphores will stay the same during application lifetime
	// Command buffer submission info is set by each example
	submitInfo = vk::SubmitInfo();
	submitInfo.pWaitDstStageMask = &submitPipelineStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &semaphores.acquireComplete;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &semaphores.renderComplete;
}

void ExampleBase::renderLoop() 
{
	DeltaTimeMeasurer timer;
	FPSMeasurer fpsMeasurer;
	fpsMeasurer.setLogFunction(1.0f, [&]() {
		const eastl::string windowTitle = 
			"VulkanExample FPS: " + StringUtils::to_string(fpsMeasurer.getAvgFps()) + 
			" MS: " + StringUtils::to_string(fpsMeasurer.getAvgMsPerFrame());
		GLEngine::graphics->setWindowTitle(windowTitle.c_str());
	});
	
	while (!GLEngine::input->isKeyPressed(EKey::Q))
	{
		GLEngine::doEngineTick();
		float deltaSec = timer.calcDeltaSec(GLEngine::getTimeMs());
		fpsMeasurer.tickFrame(deltaSec);
		render();
		update(deltaSec);
	}
}

std::string ExampleBase::getWindowTitle() {
	std::string device(deviceProperties.deviceName);
	std::string windowTitle;
	windowTitle = title + " - " + device + " - " + std::to_string(frameCounter) + " fps";
	return windowTitle;
}

std::string ExampleBase::getAssetPath() {
	return vkx::getAssetPath();
}

void ExampleBase::prepare() {
	if (enableValidation) {
		debug::setupDebugging(instance, vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning);
	}
	if (enableDebugMarkers) {
		debug::marker::setup(device);
	}
	cmdPool = getCommandPool();

	swapChain.create(size, enableVsync);
	setupDepthStencil();
	setupRenderPass();
	setupRenderPassBeginInfo();
	setupFrameBuffer();

	// Create a simple texture loader class
	textureLoader = new TextureLoader(*this);
#if defined(__ANDROID__)
	textureLoader->assetManager = androidApp->activity->assetManager;
#endif
	if (enableTextOverlay) {
		// Load the text rendering shaders
		textOverlay = new TextOverlay(*this,
			size.width,
			size.height,
			renderPass);
		updateTextOverlay();
	}
}
MeshBuffer ExampleBase::loadMesh(const std::string& filename, const MeshLayout& vertexLayout, float scale) {
	MeshLoader loader;
	loader.load(filename);
	assert(loader.m_Entries.size() > 0);
	return loader.createBuffers(*this, vertexLayout, scale);
}

vk::SubmitInfo ExampleBase::prepareSubmitInfo(
	const std::vector<vk::CommandBuffer>& commandBuffers,
	vk::PipelineStageFlags *pipelineStages) {
	vk::SubmitInfo submitInfo;
	submitInfo.pWaitDstStageMask = pipelineStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &semaphores.acquireComplete;
	submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &semaphores.renderComplete;
	return submitInfo;
}

void ExampleBase::updateTextOverlay() {
	if (!enableTextOverlay)
		return;

	textOverlay->beginTextUpdate();
	textOverlay->addText(title, 5.0f, 5.0f, TextOverlay::alignLeft);

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << (frameTimer * 1000.0f) << "ms (" << lastFPS << " fps)";
	textOverlay->addText(ss.str(), 5.0f, 25.0f, TextOverlay::alignLeft);
	textOverlay->addText(deviceProperties.deviceName, 5.0f, 45.0f, TextOverlay::alignLeft);
	getOverlayText(textOverlay);
	textOverlay->endTextUpdate();

	trashCommandBuffers(textCmdBuffers);
	populateSubCommandBuffers(textCmdBuffers, [&](const vk::CommandBuffer& cmdBuffer) {
		textOverlay->writeCommandBuffer(cmdBuffer);
	});
	primaryCmdBuffersDirty = true;
}

void ExampleBase::getOverlayText(vkx::TextOverlay *textOverlay) {
	// Can be overriden in derived class
}

void ExampleBase::prepareFrame() {
	if (primaryCmdBuffersDirty) {
		buildCommandBuffers();
	}
	// Acquire the next image from the swap chaing
	currentBuffer = swapChain.acquireNextImage(semaphores.acquireComplete);
}

void ExampleBase::submitFrame() {
	swapChain.queuePresent(semaphores.renderComplete);
	queue.waitIdle();
}

void ExampleBase::setupWindow() {
	bool fullscreen = false;

	size.width = GLEngine::graphics->getWindowWidth();
	size.height = GLEngine::graphics->getWindowHeight();

	swapChain.createSurface();
	camera.setAspectRatio(size);
}


void ExampleBase::setupDepthStencil() {
	depthStencil.destroy();

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
	vk::ImageCreateInfo image;
	image.imageType = vk::ImageType::e2D;
	image.extent = vk::Extent3D{ size.width, size.height, 1 };
	image.format = depthFormat;
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
	depthStencil = createImage(image, vk::MemoryPropertyFlagBits::eDeviceLocal);

	withPrimaryCommandBuffer([&](const vk::CommandBuffer& setupCmdBuffer) {
		setImageLayout(
			setupCmdBuffer,
			depthStencil.image,
			aspect,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal);
	});


	vk::ImageViewCreateInfo depthStencilView;
	depthStencilView.viewType = vk::ImageViewType::e2D;
	depthStencilView.format = depthFormat;
	depthStencilView.subresourceRange.aspectMask = aspect;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.layerCount = 1;
	depthStencilView.image = depthStencil.image;
	depthStencil.view = device.createImageView(depthStencilView);
}

void ExampleBase::setupFrameBuffer() {
	// Recreate the frame buffers
	if (!framebuffers.empty()) {
		for (uint32_t i = 0; i < framebuffers.size(); i++) {
			device.destroyFramebuffer(framebuffers[i]);
		}
		framebuffers.clear();
	}

	vk::ImageView attachments[2];

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = depthStencil.view;

	vk::FramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.renderPass = renderPass;
	framebufferCreateInfo.attachmentCount = 2;
	framebufferCreateInfo.pAttachments = attachments;
	framebufferCreateInfo.width = size.width;
	framebufferCreateInfo.height = size.height;
	framebufferCreateInfo.layers = 1;

	// Create frame buffers for every swap chain image
	framebuffers = swapChain.createFramebuffers(framebufferCreateInfo);
}

void ExampleBase::setupRenderPass() {
	if (renderPass) {
		device.destroyRenderPass(renderPass);
	}

	std::vector<vk::AttachmentDescription> attachments;
	attachments.resize(2);

	// Color attachment
	attachments[0].format = colorformat;
	attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	attachments[0].initialLayout = vk::ImageLayout::eUndefined;
	attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

	// Depth attachment
	attachments[1].format = depthFormat;
	attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].initialLayout = vk::ImageLayout::eUndefined;
	attachments[1].finalLayout = vk::ImageLayout::eUndefined;

	// Only one depth attachment, so put it first in the references
	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	std::vector<vk::AttachmentReference> colorAttachmentReferences;
	{
		vk::AttachmentReference colorReference;
		colorReference.attachment = 0;
		colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
		colorAttachmentReferences.push_back(colorReference);
	}

	std::vector<vk::SubpassDescription> subpasses;
	std::vector<vk::SubpassDependency> subpassDependencies;
	{
		vk::SubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
		subpassDependencies.push_back(dependency);

		vk::SubpassDescription subpass;
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.pDepthStencilAttachment = &depthReference;
		subpass.colorAttachmentCount = colorAttachmentReferences.size();
		subpass.pColorAttachments = colorAttachmentReferences.data();
		subpasses.push_back(subpass);
	}

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = subpassDependencies.size();
	renderPassInfo.pDependencies = subpassDependencies.data();
	renderPass = device.createRenderPass(renderPassInfo);
}

void ExampleBase::windowResize(const glm::uvec2& newSize) {
	if (!prepared) {
		return;
	}
	prepared = false;

	queue.waitIdle();
	device.waitIdle();

	// Recreate swap chain
	size.width = newSize.x;
	size.height = newSize.y;
	camera.setAspectRatio(size);
	swapChain.create(size, enableVsync);

	setupDepthStencil();
	setupFrameBuffer();
	if (enableTextOverlay && textOverlay->visible) {
		updateTextOverlay();
	}
	setupRenderPassBeginInfo();

	// Notify derived class
	windowResized();

	// Can be overriden in derived class
	updateDrawCommandBuffers();

	// Command buffers need to be recreated as they may store
	// references to the recreated frame buffer
	buildCommandBuffers();

	viewChanged();

	prepared = true;
}

void ExampleBase::windowResized() {}

