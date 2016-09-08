/*
* Vulkan Example - Using different pipelines in one single renderpass
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "vulkanExampleBase.h"


// Vertex layout for this example
std::vector<vkx::VertexLayout> vertexLayout =
{
	vkx::VertexLayout::VERTEX_LAYOUT_POSITION,
	vkx::VertexLayout::VERTEX_LAYOUT_NORMAL,
	vkx::VertexLayout::VERTEX_LAYOUT_UV,
	vkx::VertexLayout::VERTEX_LAYOUT_COLOR
};

static vk::PhysicalDeviceFeatures features = [] {
	vk::PhysicalDeviceFeatures features;
	features.wideLines = VK_TRUE;
	return features;
}();

class VulkanExample : public vkx::ExampleBase {
public:
	struct {
		vk::PipelineVertexInputStateCreateInfo inputState;
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		vkx::MeshBuffer cube;
	} meshes;

	vkx::UniformData uniformDataVS;

	// Same uniform buffer layout as shader
	struct UboVS {
		glm::mat4 projection;
		glm::mat4 modelView;
		glm::vec4 lightPos = glm::vec4(0.0f, 2.0f, 1.0f, 0.0f);
	} uboVS;

	vk::PipelineLayout pipelineLayout;
	vk::DescriptorSet descriptorSet;
	vk::DescriptorSetLayout descriptorSetLayout;

	struct {
		vk::Pipeline phong;
		vk::Pipeline wireframe;
		vk::Pipeline toon;
	} pipelines;

	VulkanExample() : vkx::ExampleBase(ENABLE_VALIDATION) {
		camera.setZoom(-10.5f);
		camera.setRotation({ -25.0f, 15.0f, 0.0f });
		enableTextOverlay = true;
		title = "Vulkan Example - vk::Pipeline state objects";
	}

	~VulkanExample() {
		// Clean up used Vulkan resources 
		// Note : Inherited destructor cleans up resources stored in base class
		device.destroyPipeline(pipelines.phong);
		if (deviceFeatures.fillModeNonSolid) {
			device.destroyPipeline(pipelines.wireframe);
		}
		device.destroyPipeline(pipelines.toon);

		device.destroyPipelineLayout(pipelineLayout);
		device.destroyDescriptorSetLayout(descriptorSetLayout);

		meshes.cube.destroy();

		device.destroyBuffer(uniformDataVS.buffer);
		device.freeMemory(uniformDataVS.memory);
	}

	void updateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) {

		cmdBuffer.setScissor(0, vkx::rect2D(size));
		cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
		cmdBuffer.bindVertexBuffers(VERTEX_BUFFER_BIND_ID, meshes.cube.vertices.buffer, { 0 });
		cmdBuffer.bindIndexBuffer(meshes.cube.indices.buffer, 0, vk::IndexType::eUint32);

		// Left : Solid colored 
		vk::Viewport viewport = vkx::viewport((float)size.width / 3, (float)size.height, 0.0f, 1.0f);
		cmdBuffer.setViewport(0, viewport);
		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.phong);

		cmdBuffer.drawIndexed(meshes.cube.indexCount, 1, 0, 0, 0);

		// Center : Toon
		viewport.x += viewport.width;
		cmdBuffer.setViewport(0, viewport);
		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.toon);
		cmdBuffer.setLineWidth(2.0f);
		cmdBuffer.drawIndexed(meshes.cube.indexCount, 1, 0, 0, 0);

		auto lineWidthGranularity = deviceProperties.limits.lineWidthGranularity;
		auto lineWidthRange = deviceProperties.limits.lineWidthRange;

		if (deviceFeatures.fillModeNonSolid) {
			// Right : Wireframe 
			viewport.x += viewport.width;
			cmdBuffer.setViewport(0, viewport);
			cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.wireframe);
			cmdBuffer.drawIndexed(meshes.cube.indexCount, 1, 0, 0, 0);
		}
	}

	void loadMeshes() {
		meshes.cube = loadMesh(getAssetPath() + "models/treasure_smooth.dae", vertexLayout, 1.0f);
	}

	void setupVertexDescriptions() {
		// Binding description
		vertices.bindingDescriptions.resize(1);
		vertices.bindingDescriptions[0] =
			vkx::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, vkx::vertexSize(vertexLayout), vk::VertexInputRate::eVertex);

		// Attribute descriptions
		// Describes memory layout and shader positions
		vertices.attributeDescriptions.resize(4);
		// Location 0 : Position
		vertices.attributeDescriptions[0] =
			vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 0, vk::Format::eR32G32B32Sfloat, 0);
		// Location 1 : Color
		vertices.attributeDescriptions[1] =
			vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 1, vk::Format::eR32G32B32Sfloat, sizeof(float) * 3);
		// Location 3 : Texture coordinates
		vertices.attributeDescriptions[2] =
			vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 2, vk::Format::eR32G32Sfloat, sizeof(float) * 6);
		// Location 2 : Normal
		vertices.attributeDescriptions[3] =
			vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 3, vk::Format::eR32G32B32Sfloat, sizeof(float) * 8);

		vertices.inputState = vk::PipelineVertexInputStateCreateInfo();
		vertices.inputState.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
		vertices.inputState.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
		vertices.inputState.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
		vertices.inputState.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
	}

	void setupDescriptorPool() {
		std::vector<vk::DescriptorPoolSize> poolSizes =
		{
			vkx::descriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
		};

		vk::DescriptorPoolCreateInfo descriptorPoolInfo =
			vkx::descriptorPoolCreateInfo(poolSizes.size(), poolSizes.data(), 2);

		descriptorPool = device.createDescriptorPool(descriptorPoolInfo);
	}

	void setupDescriptorSetLayout() {
		std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings =
		{
			// Binding 0 : Vertex shader uniform buffer
			vkx::descriptorSetLayoutBinding(
				vk::DescriptorType::eUniformBuffer,
				vk::ShaderStageFlagBits::eVertex,
				0)
		};

		vk::DescriptorSetLayoutCreateInfo descriptorLayout =
			vkx::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), setLayoutBindings.size());

		descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout);

		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
			vkx::pipelineLayoutCreateInfo(&descriptorSetLayout, 1);

		pipelineLayout = device.createPipelineLayout(pPipelineLayoutCreateInfo);
	}

	void setupDescriptorSet() {
		vk::DescriptorSetAllocateInfo allocInfo =
			vkx::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);

		descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

		std::vector<vk::WriteDescriptorSet> writeDescriptorSets =
		{
			// Binding 0 : Vertex shader uniform buffer
			vkx::writeDescriptorSet(
				descriptorSet,
				vk::DescriptorType::eUniformBuffer,
				0,
				&uniformDataVS.descriptor)
		};

		device.updateDescriptorSets(writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
	}

	void preparePipelines() {
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState =
			vkx::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, vk::PipelineInputAssemblyStateCreateFlags(), VK_FALSE);

		vk::PipelineRasterizationStateCreateInfo rasterizationState =
			vkx::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise);

		vk::PipelineColorBlendAttachmentState blendAttachmentState =
			vkx::pipelineColorBlendAttachmentState();

		vk::PipelineColorBlendStateCreateInfo colorBlendState =
			vkx::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

		vk::PipelineDepthStencilStateCreateInfo depthStencilState =
			vkx::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual);

		vk::PipelineViewportStateCreateInfo viewportState =
			vkx::pipelineViewportStateCreateInfo(1, 1);

		vk::PipelineMultisampleStateCreateInfo multisampleState =
			vkx::pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

		std::vector<vk::DynamicState> dynamicStateEnables = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
			vk::DynamicState::eLineWidth,
		};
		vk::PipelineDynamicStateCreateInfo dynamicState =
			vkx::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), dynamicStateEnables.size());

		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

		// Phong shading pipeline
		shaderStages[0] = loadShader(getAssetPath() + "shaders/pipelines/phong.vert.spv", vk::ShaderStageFlagBits::eVertex);
		shaderStages[1] = loadShader(getAssetPath() + "shaders/pipelines/phong.frag.spv", vk::ShaderStageFlagBits::eFragment);

		vk::GraphicsPipelineCreateInfo pipelineCreateInfo =
			vkx::pipelineCreateInfo(pipelineLayout, renderPass);

		pipelineCreateInfo.pVertexInputState = &vertices.inputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.stageCount = shaderStages.size();
		pipelineCreateInfo.pStages = shaderStages.data();

		// We are using this pipeline as the base for the other pipelines (derivatives)
		// vk::Pipeline derivatives can be used for pipelines that share most of their state
		// Depending on the implementation this may result in better performance for pipeline 
		// switchting and faster creation time
		pipelineCreateInfo.flags = vk::PipelineCreateFlagBits::eAllowDerivatives;

		// Textured pipeline
		pipelines.phong = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];

		// All pipelines created after the base pipeline will be derivatives
		pipelineCreateInfo.flags = vk::PipelineCreateFlagBits::eDerivative;
		// Base pipeline will be our first created pipeline
		pipelineCreateInfo.basePipelineHandle = pipelines.phong;
		// It's only allowed to either use a handle or index for the base pipeline
		// As we use the handle, we must set the index to -1 (see section 9.5 of the specification)
		pipelineCreateInfo.basePipelineIndex = -1;

		// Toon shading pipeline
		shaderStages[0] = loadShader(getAssetPath() + "shaders/pipelines/toon.vert.spv", vk::ShaderStageFlagBits::eVertex);
		shaderStages[1] = loadShader(getAssetPath() + "shaders/pipelines/toon.frag.spv", vk::ShaderStageFlagBits::eFragment);

		pipelines.toon = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];

		// Non solid rendering is not a mandatory Vulkan feature
		if (deviceFeatures.fillModeNonSolid) {
			// vk::Pipeline for wire frame rendering
			rasterizationState.polygonMode = vk::PolygonMode::eLine;
			shaderStages[0] = loadShader(getAssetPath() + "shaders/pipelines/wireframe.vert.spv", vk::ShaderStageFlagBits::eVertex);
			shaderStages[1] = loadShader(getAssetPath() + "shaders/pipelines/wireframe.frag.spv", vk::ShaderStageFlagBits::eFragment);
			pipelines.wireframe = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];
		}
	}

	// Prepare and initialize uniform buffer containing shader uniforms
	void prepareUniformBuffers() {
		// Create the vertex shader uniform buffer block
		uniformDataVS = createUniformBuffer(uboVS);
		updateUniformBuffers();
	}

	void updateUniformBuffers() {
		uboVS.projection = glm::perspective(glm::radians(60.0f), (float)(size.width / 3.0f) / (float)size.height, 0.001f, 256.0f);
		uboVS.modelView = camera.matrices.view;
		uniformDataVS.copy(uboVS);
	}

	void prepare() {
		ExampleBase::prepare();
		loadMeshes();
		setupVertexDescriptions();
		prepareUniformBuffers();
		setupDescriptorSetLayout();
		preparePipelines();
		setupDescriptorPool();
		setupDescriptorSet();
		updateDrawCommandBuffers();
		prepared = true;
	}

	void render() override {
		if (!prepared)
			return;
		draw();
	}

	void viewChanged() override {
		updateUniformBuffers();
	}

	void getOverlayText(vkx::TextOverlay *textOverlay) override {
		textOverlay->addText("Phong shading pipeline", (float)size.width / 6.0f, size.height - 35.0f, vkx::TextOverlay::alignCenter);
		textOverlay->addText("Toon shading pipeline", (float)size.width / 2.0f, size.height - 35.0f, vkx::TextOverlay::alignCenter);
		textOverlay->addText("Wireframe pipeline", size.width - (float)size.width / 6.5f, size.height - 35.0f, vkx::TextOverlay::alignCenter);
	}
};