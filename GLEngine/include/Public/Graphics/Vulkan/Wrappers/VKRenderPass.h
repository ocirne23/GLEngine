#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;
class VKCommandBuffer;
class VKBuffer;
class VKPipeline;

class VKRenderPass
{
public:

	VKRenderPass() {}
	VKRenderPass(const VKRenderPass& copy) { assert(!m_initialized); }
	~VKRenderPass();
	void initialize(VKDevice& device, vk::Format colorFormat, vk::Format depthFormat);
	void cleanup();


	void beginCmd(VKCommandBuffer& commandBuffer);

	void bindPipelineDescriptorCmd(VKPipeline& pipeline);
	void bindVertexBufferCmd(VKBuffer& buffer, uint bindID);
	void bindIndexBufferCmd(VKBuffer& buffer, vk::IndexType indexType);
	void drawIndexedCmd(uint numIndices);
	void prePresentBarrierCmd(vk::Image image);

	void endCmd();


	bool isInitialized() const { return m_initialized; }
	vk::RenderPass getVKRenderPass() { return m_renderPass; }

private:

	bool m_initialized = false;
	bool m_begun = false;
	vk::RenderPass m_renderPass = VK_NULL_HANDLE;
	vk::CommandBuffer m_commandBuffer = VK_NULL_HANDLE;
};