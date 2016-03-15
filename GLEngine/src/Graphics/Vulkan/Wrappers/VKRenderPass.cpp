#include "Graphics/Vulkan/Wrappers/VKRenderPass.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Wrappers/VKBuffer.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"
#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKPipeline.h"
#include "Graphics/Vulkan/Wrappers/VKFramebuffer.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKRenderPass::~VKRenderPass()
{
	if (m_initialized)
		cleanup();
}

void VKRenderPass::initialize(VKDevice& a_device, vk::Format a_colorFormat, vk::Format a_depthFormat)
{
	if (m_initialized)
		cleanup();

	vk::AttachmentDescription attachments[2];
	attachments[0].format(a_colorFormat);
	attachments[0].samples(vk::SampleCountFlagBits::e1);
	attachments[0].loadOp(vk::AttachmentLoadOp::eClear);
	attachments[0].storeOp(vk::AttachmentStoreOp::eStore);
	attachments[0].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	attachments[0].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[0].initialLayout(vk::ImageLayout::eColorAttachmentOptimal);
	attachments[0].finalLayout(vk::ImageLayout::eColorAttachmentOptimal);

	attachments[1].format(a_depthFormat);
	attachments[1].samples(vk::SampleCountFlagBits::e1);
	attachments[1].loadOp(vk::AttachmentLoadOp::eClear);
	attachments[1].storeOp(vk::AttachmentStoreOp::eStore);
	attachments[1].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	attachments[1].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[1].initialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
	attachments[1].finalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference colorReference = vk::AttachmentReference()
		.attachment(0)
		.layout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentReference depthReference = vk::AttachmentReference()
		.attachment(1)
		.layout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass = vk::SubpassDescription()
		.pipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.flags(0)
		.inputAttachmentCount(0)
		.pInputAttachments(NULL)
		.colorAttachmentCount(1)
		.pColorAttachments(&colorReference)
		.pResolveAttachments(NULL)
		.pDepthStencilAttachment(&depthReference)
		.preserveAttachmentCount(0)
		.pPreserveAttachments(NULL);

	vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
		.attachmentCount(2)
		.pAttachments(attachments)
		.subpassCount(1)
		.pSubpasses(&subpass)
		.dependencyCount(0)
		.pDependencies(NULL);

	VKVerifier result = vk::createRenderPass(a_device.getVKDevice(), &renderPassCreateInfo, NULL, &m_renderPass);

	m_initialized = true;
}

void VKRenderPass::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}

void VKRenderPass::beginCmd(VKCommandBuffer& a_commandBuffer, VKFramebuffer& a_framebuffer)
{
	assert(m_initialized);
	assert(!m_begun);
	assert(a_commandBuffer.isBegun());

	m_commandBuffer = a_commandBuffer.getVKCommandBuffer();

	uint width = GLEngine::graphics->getWindowWidth();
	uint height = GLEngine::graphics->getWindowHeight();
	
	eastl::fixed_vector<float, 4> col = { 0.025f, 0.025f, 0.025f, 1.0f };	
	vk::ClearColorValue clearColor = vk::ClearColorValue()
		.float32(col);

	vk::ClearDepthStencilValue clearDepthStencil = vk::ClearDepthStencilValue()
		.depth(1.0f)
		.stencil(0);

	vk::CommandBufferBeginInfo cmdBufInfo = vk::CommandBufferBeginInfo();
	vk::ClearValue clearValues[2];
	clearValues[0].color(clearColor);
	clearValues[1].depthStencil(clearDepthStencil);

	vk::RenderPassBeginInfo renderpassBeginInfo = vk::RenderPassBeginInfo()
		.renderPass(m_renderPass)
		.renderArea(vk::Rect2D()
			.extent(vk::Extent2D(width, height))
			.offset(vk::Offset2D(0, 0)))
		.clearValueCount(2)
		.pClearValues(clearValues)
		.framebuffer(a_framebuffer.getVKFramebuffer());

	vk::cmdBeginRenderPass(m_commandBuffer, renderpassBeginInfo, vk::SubpassContents::eInline);

	m_begun = true;
}

void VKRenderPass::bindPipelineDescriptorCmd(VKPipeline& a_pipeline)
{
	assert(m_initialized);
	assert(m_begun);

	uint width = GLEngine::graphics->getWindowWidth();
	uint height = GLEngine::graphics->getWindowHeight();

	// Update dynamic viewport state
	vk::Viewport viewport = vk::Viewport()
		.width(float(width))
		.height(float(height))
		.minDepth(0.0f)
		.maxDepth(1.0f);
	vk::cmdSetViewport(m_commandBuffer, 0, 1, &viewport);

	vk::Rect2D scissor = vk::Rect2D()
		.extent(vk::Extent2D(width, height))
		.offset(vk::Offset2D(0, 0));
	vk::cmdSetScissor(m_commandBuffer, 0, 1, &scissor);

	vk::DescriptorSet descriptorSet = a_pipeline.getVKDescriptorSet();
	vk::cmdBindDescriptorSets(m_commandBuffer, vk::PipelineBindPoint::eGraphics, a_pipeline.getVKPipelineLayout(), 0, 1, &descriptorSet, 0, NULL);
	vk::cmdBindPipeline(m_commandBuffer, vk::PipelineBindPoint::eGraphics, a_pipeline.getVKPipeline());
}

void VKRenderPass::bindVertexBufferCmd(VKBuffer& a_buffer, uint a_bindID)
{
	assert(m_initialized);
	assert(m_begun);

	vk::DeviceSize offsets[] = { 0 };
	vk::Buffer buffer = a_buffer.getVKBuffer();
	vk::cmdBindVertexBuffers(m_commandBuffer, a_bindID, 1, &buffer, offsets);
}

void VKRenderPass::bindIndexBufferCmd(VKBuffer& a_buffer, vk::IndexType a_indexType)
{
	assert(m_initialized);
	assert(m_begun);
	vk::cmdBindIndexBuffer(m_commandBuffer, a_buffer.getVKBuffer(), 0, a_indexType);
}

void VKRenderPass::drawIndexedCmd(uint a_numIndices)
{
	assert(m_initialized);
	assert(m_begun);
	vk::cmdDrawIndexed(m_commandBuffer, a_numIndices, 1, 0, 0, 1);
}

void VKRenderPass::prePresentBarrierCmd(vk::Image a_image)
{
	assert(m_initialized);
	assert(m_begun);

	vk::ImageMemoryBarrier prePresentBarrier = vk::ImageMemoryBarrier()
		.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
		.dstAccessMask(vk::AccessFlags())
		.oldLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.newLayout(vk::ImageLayout::ePresentSrcKhr)
		.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.subresourceRange(vk::ImageSubresourceRange()
			.aspectMask(vk::ImageAspectFlagBits::eColor)
			.baseMipLevel(0)
			.levelCount(1)
			.baseArrayLayer(0)
			.layerCount(1))
		.image(a_image);

	vk::cmdPipelineBarrier(
		m_commandBuffer,
		vk::PipelineStageFlagBits::eAllCommands,
		vk::PipelineStageFlagBits::eTopOfPipe,
		vk::DependencyFlags(),
		0, NULL,
		0, NULL,
		1, &prePresentBarrier);
}

void VKRenderPass::endCmd()
{
	assert(m_initialized);
	assert(m_begun);
	vk::cmdEndRenderPass(m_commandBuffer);
	m_commandBuffer = VK_NULL_HANDLE;
	m_begun = false;
}
