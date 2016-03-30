#include "Graphics/Vulkan/Wrappers/VKPipeline.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKRenderPass.h"
#include "Graphics/Vulkan/Wrappers/VKPipelineDescription.h"

#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Utils/FileHandle.h"

BEGIN_UNNAMED_NAMESPACE()

VkShaderModule loadShaderModule(const char* a_fileName, VKDevice& a_device)
{
	FileHandle file(a_fileName);
	assert(file.exists());
	uint64 size = file.getFileSize();
	eastl::vector<char> data;
	data.reserve(size);
	file.readBytes(&data[0], size, 0);

	vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
		.codeSize(size)
		.pCode(rcast<const uint32_t*>(&data[0]));
	
	vk::ShaderModule shaderModule;
	VKVerifier result = vk::createShaderModule(a_device.getVKDevice(), &moduleCreateInfo, NULL, &shaderModule);
	return shaderModule;
}

END_UNNAMED_NAMESPACE()

void VKPipeline::initialize(VKDevice& a_device, VKRenderPass& a_renderPass, VKPipelineDescription& a_pipelineDescription)
{
	VKVerifier result;
	/// createPipelineCache

	vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = vk::PipelineCacheCreateInfo();
	result = vk::createPipelineCache(a_device.getVKDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache);

	/// setupDescriptorSetLayout

	vk::DescriptorSetLayoutBinding layoutBinding = vk::DescriptorSetLayoutBinding()
		.descriptorType(vk::DescriptorType::eUniformBuffer)
		.descriptorCount(1)
		.stageFlags(vk::ShaderStageFlagBits::eVertex);

	vk::DescriptorSetLayoutCreateInfo descriptorLayout = vk::DescriptorSetLayoutCreateInfo()
		.bindingCount(1)
		.pBindings(&layoutBinding);

	result = vk::createDescriptorSetLayout(a_device.getVKDevice(), &descriptorLayout, NULL, &m_descriptorSetLayout);

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
		.setLayoutCount(1)
		.pSetLayouts(&m_descriptorSetLayout);

	result = vk::createPipelineLayout(a_device.getVKDevice(), &pipelineLayoutCreateInfo, NULL, &m_pipelineLayout);

	/// preparePipelines

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo()
		.topology(vk::PrimitiveTopology::eTriangleList); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	vk::PipelineRasterizationStateCreateInfo rasterizationState = vk::PipelineRasterizationStateCreateInfo()
		.polygonMode(vk::PolygonMode::eFill)
		.cullMode(vk::CullModeFlagBits::eNone)
		.frontFace(vk::FrontFace::eCounterClockwise)
		.depthClampEnable(VK_FALSE)
		.rasterizerDiscardEnable(VK_FALSE)
		.depthBiasEnable(VK_FALSE);

	vk::PipelineColorBlendAttachmentState blendAttachmentState[1] = {};
	blendAttachmentState[0] = vk::PipelineColorBlendAttachmentState()
		.colorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA)
		.blendEnable(VK_FALSE);

	vk::PipelineColorBlendStateCreateInfo colorBlendState = vk::PipelineColorBlendStateCreateInfo()
		.attachmentCount(1)
		.pAttachments(blendAttachmentState);

	vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo()
		.viewportCount(1)
		.scissorCount(1);

	vk::DynamicState dynamicStates[] = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	vk::PipelineDynamicStateCreateInfo dynamicState = vk::PipelineDynamicStateCreateInfo()
		.dynamicStateCount(ARRAY_SIZE(dynamicStates))
		.pDynamicStates(dynamicStates);

	vk::PipelineDepthStencilStateCreateInfo depthStencilState = vk::PipelineDepthStencilStateCreateInfo()
		.depthTestEnable(VK_TRUE)
		.depthWriteEnable(VK_TRUE)
		.depthCompareOp(vk::CompareOp::eLessOrEqual)
		.depthBoundsTestEnable(VK_FALSE)
		.back(vk::StencilOpState()
			.failOp(vk::StencilOp::eKeep)
			.passOp(vk::StencilOp::eKeep)
			.compareOp(vk::CompareOp::eAlways))
		.stencilTestEnable(VK_FALSE)
		.front(vk::StencilOpState()
			.failOp(vk::StencilOp::eKeep)
			.passOp(vk::StencilOp::eKeep)
			.compareOp(vk::CompareOp::eAlways));

	vk::PipelineMultisampleStateCreateInfo multisampleState = vk::PipelineMultisampleStateCreateInfo()
		.rasterizationSamples(vk::SampleCountFlagBits::e1);

	vk::PipelineShaderStageCreateInfo shaderStages[2] = {};
	shaderStages[0] = vk::PipelineShaderStageCreateInfo()
		.stage(vk::ShaderStageFlagBits::eVertex)
		.module(loadShaderModule(a_pipelineDescription.m_vertexShaderPath.c_str(), a_device))
		.pName("main");

	shaderStages[1] = vk::PipelineShaderStageCreateInfo()
		.stage(vk::ShaderStageFlagBits::eFragment)
		.module(loadShaderModule(a_pipelineDescription.m_fragmentShaderPath.c_str(), a_device))
		.pName("main");

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.layout(m_pipelineLayout)
		.pVertexInputState(&a_pipelineDescription.m_vertexInputState)
		.pInputAssemblyState(&inputAssemblyState)
		.pRasterizationState(&rasterizationState)
		.pColorBlendState(&colorBlendState)
		.pMultisampleState(&multisampleState)
		.pViewportState(&viewportState)
		.pDepthStencilState(&depthStencilState)
		.stageCount(ARRAY_SIZE(shaderStages))
		.pStages(shaderStages)
		.renderPass(a_renderPass.getVKRenderPass())
		.pDynamicState(&dynamicState);

	result = vk::createGraphicsPipelines(a_device.getVKDevice(), m_pipelineCache, 1, &pipelineCreateInfo, NULL, &m_pipeline);

	/// setupDescriptorPool

	vk::DescriptorPoolSize typeCounts[1] = {};
	typeCounts[0] = vk::DescriptorPoolSize()
		.type(vk::DescriptorType::eUniformBuffer)
		.descriptorCount(1);

	vk::DescriptorPoolCreateInfo descriptorPoolInfo = vk::DescriptorPoolCreateInfo()
		.poolSizeCount(1)
		.pPoolSizes(typeCounts)
		.maxSets(1);

	result = vk::createDescriptorPool(a_device.getVKDevice(), &descriptorPoolInfo, NULL, &m_descriptorPool);

	/// setupDescriptorSet

	vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo()
		.descriptorPool(m_descriptorPool)
		.descriptorSetCount(1)
		.pSetLayouts(&m_descriptorSetLayout);

	result = vk::allocateDescriptorSets(a_device.getVKDevice(), &allocInfo, &m_descriptorSet);

	vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
		.descriptorCount(1)
		.descriptorType(vk::DescriptorType::eUniformBuffer)
		.pBufferInfo(&a_pipelineDescription.m_bufferInfo)
		.dstSet(m_descriptorSet)
		.dstBinding(0);

	vk::updateDescriptorSets(a_device.getVKDevice(), 1, &writeDescriptorSet, 0, NULL);
}
