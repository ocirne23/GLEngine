/*
* Vulkan Example - Basic indexed triangle rendering
*
* Note :
*    This is a "pedal to the metal" example to show off how to get Vulkan up an displaying something
*    Contrary to the other examples, this one won't make use of helper functions or initializers
*    Except in a few cases (swap chain setup e.g.)
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include "base/vulkanExampleBase.h"

class VulkanExample : public vkx::ExampleBase {
	// Moved to base class
	/*
	GLFWwindow* window{ nullptr };
	float zoom{ -2.5f };
	std::string title{ "Vulkan Example - Basic indexed triangle" };
	vk::Extent2D size{ 1280, 720 };
	vkx::SwapChain swapChain;
	uint32_t currentBuffer;
	vk::CommandPool cmdPool;
	vk::DescriptorPool descriptorPool;
	vk::RenderPass renderPass;
	// List of shader modules created (stored for cleanup)
	std::vector<vk::ShaderModule> shaderModules;
	// List of available frame buffers (same as number of swap chain images)
	std::vector<vk::Framebuffer> framebuffers;
	std::vector<vk::CommandBuffer> cmdBuffers;
	std::vector<vk::Fence> submitFences;
	bool prepared{ false };
	*/

	// Moved to base class
	/*
	void run()
	void render()
	void prepare()
	void setupRenderPass()
	void setupFrameBuffer()
	void prepareSemaphore()
	*/

public:
	// CreateBufferResult is a helper structure to encapsulate a buffer, 
	// the memory for that buffer, and a descriptor for the buffer (if necessary)
	// We'll see more of what it does when we start using it
	//
	// We need one for vertices, one for indices, and one for uniform data
	CreateBufferResult vertices;
	CreateBufferResult indices;
	CreateBufferResult uniformDataVS;
	uint32_t indexCount{ 0 };

	// As before
	vk::DescriptorSet descriptorSet;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::Pipeline pipeline;
	vk::PipelineLayout pipelineLayout;
	vk::PipelineVertexInputStateCreateInfo inputState;
	std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

	// As before
	struct UboVS {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uboVS;

	VulkanExample() : vkx::ExampleBase(ENABLE_VALIDATION) {
		size.width = 1280;
		size.height = 720;
		camera.setZoom(-2.5f);
		title = "Vulkan Example - triangle revisited";
	}

	~VulkanExample() {
		// The helper class we use for encapsulating buffer has a destroy method
		// that cleans up all the resources it owns.  
		vertices.destroy();
		indices.destroy();
		uniformDataVS.destroy();

		// As before
		device.destroyPipeline(pipeline);
		device.destroyPipelineLayout(pipelineLayout);
		device.destroyDescriptorSetLayout(descriptorSetLayout);
	}

	void prepare() {
		// Even though we moved some of the preparations to the base class, we still have more to do locally
		// so we call be base class prepare method and then our preparation methods.  The base class sets up
		// the swapchain, renderpass, framebuffers, command pool and debugging.  It also creates some 
		// helper classes for loading textures and for rendering text overlays, but we will not use them yet
		ExampleBase::prepare();
		prepareVertices();
		prepareUniformBuffers();
		setupDescriptorSetLayout();
		preparePipelines();
		setupDescriptorPool();
		setupDescriptorSet();

		// Update the drawCmdBuffers with the required drawing commands
		updateDrawCommandBuffers();
		prepared = true;
	}

	// In our previous example, we created a function buildDrawCommandBuffers that did two jobs.  First, it allocated a 
	// command buffer for each swapChain image, and then it populated those command buffers with the commands required
	// to render our triangle.
	//
	// The example base class works a little differently.  First, the act of allocating the command buffers is boilerplate
	// so it's moved to base class.  Second, the common examples base class actually contains multiple sets of command 
	// buffers.  The outermost are called the primaryCmdBuffers and are the ones we actually execute for each frame.  
	//
	// However, the primaryCmdBuffers do not contain drawing commands.  Instead, drawing commands are recorded into 
	// secondary command puffers, while the primaryCmdBuffers just execute secondary command buffers.  
	// 
	// This updateDrawCommandBuffer method is intended to populate a commandBuffer with all the commands required for 
	// rendering.  The parent class has already set up all the basic command begin and beginRenderPass calls, and 
	// will also take care of the endRenderPass and end calls.  
	// 
	// All that's left for us to do is to bind pipelines, set drawing regions and draw geometry.
	void updateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) override {
		cmdBuffer.setViewport(0, vkx::viewport(size));
		cmdBuffer.setScissor(0, vkx::rect2D(size));
		cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		cmdBuffer.bindVertexBuffers(VERTEX_BUFFER_BIND_ID, vertices.buffer, { 0 });
		cmdBuffer.bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);
		cmdBuffer.drawIndexed(indexCount, 1, 0, 0, 1);
	}

	// The prepareVertices method has changed from the previous implementation.  All of the logic that was done to
	// populate the device local buffers has been moved into a helper function, "stageToDeviceBuffer"
	//
	// stageToDeviceBuffer takes care of all of the work of creating a temporary host visible buffer, copying the 
	// data to it, creating the actual device local buffer, copying the contents from one buffer to another, 
	// and destroying the temporary buffer.  
	// 
	// Additionally, the staging function is templated, so you don't need to pass it a void pointer and a size,
	// but instead can pass it a std::vector containing an array of data, and it will automatically calculate 
	// the required size.
	void prepareVertices() {
		struct Vertex {
			float pos[3];
			float col[3];
		};

		// Setup vertices
		std::vector<Vertex> vertexBuffer = {
			{ { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
		};
		vertices = stageToDeviceBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vertexBuffer);

		// Setup indices
		std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
		indexCount = (uint32_t)indexBuffer.size();
		indices = stageToDeviceBuffer(vk::BufferUsageFlagBits::eIndexBuffer, indexBuffer);

		// As before
		bindingDescriptions.resize(1);
		bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;
		attributeDescriptions.resize(2);
		attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = 0;
		attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = sizeof(float) * 3;
		inputState.vertexBindingDescriptionCount = bindingDescriptions.size();
		inputState.pVertexBindingDescriptions = bindingDescriptions.data();
		inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();
		inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
	}

	////////////////////////////////////////
	//
	// All as before
	//
	void prepareUniformBuffers() {
		uboVS.projectionMatrix = getProjection();
		uboVS.viewMatrix = glm::translate(glm::mat4(), camera.position);
		uboVS.modelMatrix = glm::mat4_cast(camera.orientation);
		uniformDataVS = createUniformBuffer(uboVS);
	}

	void setupDescriptorSetLayout() {
		// Setup layout of descriptors used in this example
		// Basically connects the different shader stages to descriptors
		// for binding uniform buffers, image samplers, etc.
		// So every shader binding should map to one descriptor set layout
		// binding

		// Binding 0 : Uniform buffer (Vertex shader)
		vk::DescriptorSetLayoutBinding layoutBinding;
		layoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
		layoutBinding.pImmutableSamplers = NULL;

		vk::DescriptorSetLayoutCreateInfo descriptorLayout;
		descriptorLayout.bindingCount = 1;
		descriptorLayout.pBindings = &layoutBinding;

		descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout, NULL);

		// Create the pipeline layout that is used to generate the rendering pipelines that
		// are based on this descriptor set layout
		// In a more complex scenario you would have different pipeline layouts for different
		// descriptor set layouts that could be reused
		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
		pPipelineLayoutCreateInfo.setLayoutCount = 1;
		pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayout = device.createPipelineLayout(pPipelineLayoutCreateInfo);
	}

	void preparePipelines() {
		// Create our rendering pipeline used in this example
		// Vulkan uses the concept of rendering pipelines to encapsulate
		// fixed states
		// This replaces OpenGL's huge (and cumbersome) state machine
		// A pipeline is then stored and hashed on the GPU making
		// pipeline changes much faster than having to set dozens of 
		// states
		// In a real world application you'd have dozens of pipelines
		// for every shader set used in a scene
		// Note that there are a few states that are not stored with
		// the pipeline. These are called dynamic states and the 
		// pipeline only stores that they are used with this pipeline,
		// but not their states

		vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
		// The layout used for this pipeline
		pipelineCreateInfo.layout = pipelineLayout;
		// Renderpass this pipeline is attached to
		pipelineCreateInfo.renderPass = renderPass;

		// Vertex input state
		// Describes the topoloy used with this pipeline
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
		// This pipeline renders vertex data as triangle lists
		inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;

		// Rasterization state
		vk::PipelineRasterizationStateCreateInfo rasterizationState;
		// Solid polygon mode
		rasterizationState.polygonMode = vk::PolygonMode::eFill;
		// No culling
		rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
		rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		// Color blend state
		// Describes blend modes and color masks
		vk::PipelineColorBlendStateCreateInfo colorBlendState;
		// One blend attachment state
		// Blending is not used in this example
		vk::PipelineColorBlendAttachmentState blendAttachmentState[1] = {};
		blendAttachmentState[0].colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		blendAttachmentState[0].blendEnable = VK_FALSE;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = blendAttachmentState;

		// vk::Viewport state
		vk::PipelineViewportStateCreateInfo viewportState;
		// One viewport
		viewportState.viewportCount = 1;
		// One scissor rectangle
		viewportState.scissorCount = 1;

		// Enable dynamic states
		// Describes the dynamic states to be used with this pipeline
		// Dynamic states can be set even after the pipeline has been created
		// So there is no need to create new pipelines just for changing
		// a viewport's dimensions or a scissor box
		vk::PipelineDynamicStateCreateInfo dynamicState;
		// The dynamic state properties themselves are stored in the command buffer
		std::vector<vk::DynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(vk::DynamicState::eViewport);
		dynamicStateEnables.push_back(vk::DynamicState::eScissor);
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = dynamicStateEnables.size();

		// Depth and stencil state
		// No depth testing used in this example
		vk::PipelineDepthStencilStateCreateInfo depthStencilState;

		// Multi sampling state
		// No multi sampling used in this example
		vk::PipelineMultisampleStateCreateInfo multisampleState;

		// Load shaders
		// Shaders are loaded from the SPIR-V format, which can be generated from glsl
		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;
		shaderStages[0] = loadShader(getAssetPath() + "shaders/triangle/triangle.vert.spv", vk::ShaderStageFlagBits::eVertex);
		shaderStages[1] = loadShader(getAssetPath() + "shaders/triangle/triangle.frag.spv", vk::ShaderStageFlagBits::eFragment);

		// Assign states
		// Assign pipeline state create information
		pipelineCreateInfo.stageCount = shaderStages.size();
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.pVertexInputState = &inputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.renderPass = renderPass;
		pipelineCreateInfo.pDynamicState = &dynamicState;

		// Create rendering pipeline
		pipeline = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];
	}

	void setupDescriptorPool() {
		// We need to tell the API the number of max. requested descriptors per type
		vk::DescriptorPoolSize typeCounts[1];
		// This example only uses one descriptor type (uniform buffer) and only
		// requests one descriptor of this type
		typeCounts[0].type = vk::DescriptorType::eUniformBuffer;
		typeCounts[0].descriptorCount = 1;
		// For additional types you need to add new entries in the type count list
		// E.g. for two combined image samplers :
		// typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
		// typeCounts[1].descriptorCount = 2;

		// Create the global descriptor pool
		// All descriptors used in this example are allocated from this pool
		vk::DescriptorPoolCreateInfo descriptorPoolInfo;
		descriptorPoolInfo.poolSizeCount = 1;
		descriptorPoolInfo.pPoolSizes = typeCounts;
		// Set the max. number of sets that can be requested
		// Requesting descriptors beyond maxSets will result in an error
		descriptorPoolInfo.maxSets = 1;
		descriptorPool = device.createDescriptorPool(descriptorPoolInfo);
	}

	void setupDescriptorSet() {
		// Allocate a new descriptor set from the global descriptor pool
		vk::DescriptorSetAllocateInfo allocInfo;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

		// Update the descriptor set determining the shader binding points
		// For every binding point used in a shader there needs to be one
		// descriptor set matching that binding point

		vk::WriteDescriptorSet writeDescriptorSet;

		// Binding 0 : Uniform buffer
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
		writeDescriptorSet.pBufferInfo = &uniformDataVS.descriptor;
		// Binds this uniform buffer to binding point 0
		writeDescriptorSet.dstBinding = 0;

		device.updateDescriptorSets(writeDescriptorSet, nullptr);
	}
};