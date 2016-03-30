#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKDevice;
class VKRenderPass;
class VKPipelineDescription;

class VKPipeline
{
public:

	void initialize(VKDevice& device, VKRenderPass& renderPass, VKPipelineDescription& pipelineDescription);

	vk::PipelineLayout getVKPipelineLayout() { return m_pipelineLayout; }
	vk::DescriptorSet getVKDescriptorSet() { return m_descriptorSet; }
	vk::Pipeline getVKPipeline() { return m_pipeline; }

private:

	vk::DescriptorPool m_descriptorPool;
	vk::DescriptorSetLayout m_descriptorSetLayout;
	vk::DescriptorSet m_descriptorSet;
	vk::Pipeline m_pipeline;
	vk::PipelineLayout m_pipelineLayout;
	vk::PipelineCache m_pipelineCache;
};