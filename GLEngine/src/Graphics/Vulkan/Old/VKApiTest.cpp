#include "Graphics/Vulkan/VKApiTest.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"

#include "Graphics/Vulkan/Wrappers/VKPipelineDescription.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

#include <glm/gtc/matrix_transform.hpp>

void VKApiTest::test()
{
	uint width = GLEngine::graphics->getWindowWidth();
	uint height = GLEngine::graphics->getWindowHeight();

	m_instance.initialize();
	m_physDevice = m_instance.getPhysicalDevice();
	m_device = &m_physDevice.getDevice(VKDevice::EDeviceType::Graphics);
	m_swapchain = &m_physDevice.getSwapchain();
	
	m_setupCommandBuffer.initialize(*m_device);
	m_setupCommandBuffer.begin();
	{
		m_physDevice.getSwapchain().setup(m_setupCommandBuffer, width, height);
		m_depthStencil.initialize(*m_device, m_setupCommandBuffer, width, height);
	}

	m_renderPass.initialize(*m_device, m_physDevice.getSwapchain().getColorFormat(), m_physDevice.getDepthFormat());

	uint numImages = m_physDevice.getSwapchain().getNumImages();
	m_drawCommandBuffers.resize(numImages);
	for (uint i = 0; i < numImages; ++i)
		m_drawCommandBuffers[i].initialize(*m_device);
	
	m_framebuffers.resize(numImages);
	for (uint i = 0; i < numImages; ++i)
	{
		vk::ImageView attachments[2] = {
			m_depthStencil.getView(),
			m_swapchain->getView(i)
		};
		m_framebuffers[i].initialize(*m_device, m_renderPass, { attachments, ARRAY_SIZE(attachments) }, width, height);
	}

	m_setupCommandBuffer.end();
	m_setupCommandBuffer.submit();
	m_setupCommandBuffer.waitIdle();
	m_setupCommandBuffer.initialize(*m_device);
	
	UboData uboData;
	uboData.projectionMatrix = glm::perspective(60.0f, float(width) / float(height), 0.1f, 256.0f);
	uboData.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.5f));
	uboData.modelMatrix = glm::mat4();
	Vertex vertexData[] = {
		{ glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) },
		{ glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
		{ glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) }
	};
	uint indiceData[] = {
		0, 1, 2
	};

	m_uniformBuffer.initialize(*m_device, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(uboData));
	m_vertexBuffer.initialize(*m_device,  vk::BufferUsageFlagBits::eVertexBuffer,  vk::MemoryPropertyFlagBits::eHostVisible, sizeof(vertexData));
	m_indiceBuffer.initialize(*m_device,  vk::BufferUsageFlagBits::eIndexBuffer,   vk::MemoryPropertyFlagBits::eHostVisible, sizeof(indiceData));

	UboData* ubo = rcast<UboData*>(m_uniformBuffer.mapBuffer());
		memcpy(ubo, &uboData, sizeof(uboData));
	m_uniformBuffer.unmapBuffer();

	Vertex* vertices = rcast<Vertex*>(m_vertexBuffer.mapBuffer());
		memcpy(vertices, vertexData, sizeof(vertexData));
	m_vertexBuffer.unmapBuffer();

	uint* indices = rcast<uint*>(m_indiceBuffer.mapBuffer());
		memcpy(indices, indiceData, sizeof(indiceData));
	m_indiceBuffer.unmapBuffer();

	m_uniformBuffer.bindMemory();
	m_vertexBuffer.bindMemory();
	m_indiceBuffer.bindMemory();

	vk::VertexInputAttributeDescription attributeDescriptions[] = {
		vk::VertexInputAttributeDescription()
			.location(0)
			.binding(0)
			.format(vk::Format::eR32G32B32Sfloat)
			.offset(0),
		vk::VertexInputAttributeDescription()
			.location(1)
			.binding(0)
			.format(vk::Format::eR32G32B32Sfloat)
			.offset(sizeof(glm::vec3))
	};
	vk::VertexInputBindingDescription bindingDescriptions[] = {
		vk::VertexInputBindingDescription()
		.binding(0)
		.stride(sizeof(Vertex))
		.inputRate(vk::VertexInputRate::eVertex)
	};

	VKPipelineDescription pipelineDescription;
	pipelineDescription.m_vertexShaderPath = "Shaders/vulkan/triangle.vert.spv";
	pipelineDescription.m_fragmentShaderPath = "Shaders/vulkan/triangle.frag.spv";
	pipelineDescription.m_vertexInputState = vk::PipelineVertexInputStateCreateInfo()
		.vertexAttributeDescriptionCount(ARRAY_SIZE(attributeDescriptions))
		.pVertexAttributeDescriptions(attributeDescriptions)
		.vertexBindingDescriptionCount(ARRAY_SIZE(bindingDescriptions))
		.pVertexBindingDescriptions(bindingDescriptions);
	pipelineDescription.m_bufferInfo = vk::DescriptorBufferInfo()
		.buffer(m_uniformBuffer.getVKBuffer())
		.offset(0)
		.range(sizeof(ubo));

	m_pipeline.initialize(*m_device, m_renderPass, pipelineDescription);

	for (uint i = 0; i < numImages; ++i)
	{
		m_drawCommandBuffers[i].begin();
		m_renderPass.beginCmd(m_drawCommandBuffers[i], m_framebuffers[i]);
		m_renderPass.bindPipelineDescriptorCmd(m_pipeline);
		m_renderPass.bindVertexBufferCmd(m_vertexBuffer, 0);
		m_renderPass.bindIndexBufferCmd(m_indiceBuffer, vk::IndexType::eUint32);
		m_renderPass.drawIndexedCmd(ARRAY_SIZE(indiceData));
		m_renderPass.endCmd();
		m_drawCommandBuffers[i].end();
	}

	m_postPresentCommandBuffer.initialize(*m_device);

	while (!GLEngine::input->isKeyPressed(EKey::ESCAPE))
	{
		GLEngine::doMainThreadTick();
		GLEngine::doEngineTick();

		vk::deviceWaitIdle(m_device->getVKDevice());

		VKVerifier result;
		vk::Semaphore presentCompleteSemaphore;
		vk::SemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = vk::SemaphoreCreateInfo()
			.flags(uint(vk::FenceCreateFlagBits::eSignaled));

		result = vk::createSemaphore(m_device->getVKDevice(), &presentCompleteSemaphoreCreateInfo, NULL, &presentCompleteSemaphore);
		result = vk::acquireNextImageKHR(m_device->getVKDevice(), m_swapchain->getVKSwapchain(), UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &m_currentBufferIdx);
		vk::CommandBuffer currentBuffer = m_drawCommandBuffers[m_currentBufferIdx].getVKCommandBuffer();
		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.waitSemaphoreCount(1)
			.pWaitSemaphores(&presentCompleteSemaphore)
			.commandBufferCount(1)
			.pCommandBuffers(&currentBuffer);
		result = vk::queueSubmit(m_device->getVKQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		vk::SwapchainKHR swapchain = m_swapchain->getVKSwapchain();
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.swapchainCount(1)
			.pSwapchains(&swapchain)
			.pImageIndices(&m_currentBufferIdx);
		result = vk::queuePresentKHR(m_device->getVKQueue(), presentInfo);

		vk::destroySemaphore(m_device->getVKDevice(), presentCompleteSemaphore, NULL);

		vk::ImageMemoryBarrier postPresentBarrier = vk::ImageMemoryBarrier()
			.srcAccessMask(vk::AccessFlags())
			.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.oldLayout(vk::ImageLayout::ePresentSrcKhr)
			.newLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.subresourceRange(vk::ImageSubresourceRange()
				.aspectMask(vk::ImageAspectFlagBits::eColor)
				.baseMipLevel(0)
				.levelCount(1)
				.baseArrayLayer(0)
				.layerCount(1))
			.image(m_swapchain->getImage(m_currentBufferIdx));

		m_postPresentCommandBuffer.begin();
		vk::cmdPipelineBarrier(
			m_postPresentCommandBuffer.getVKCommandBuffer(),
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlags(),
			0, NULL,
			0, NULL,
			1, &postPresentBarrier);
		m_postPresentCommandBuffer.end();
		m_postPresentCommandBuffer.submit();
		result = vk::queueWaitIdle(m_device->getVKQueue());
		result = vk::deviceWaitIdle(m_device->getVKDevice());
	}
}