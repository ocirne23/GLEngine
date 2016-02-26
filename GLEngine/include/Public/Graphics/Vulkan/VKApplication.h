#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"

#include <glm/glm.hpp>

struct DepthStencil
{
	vk::Image image;
	vk::DeviceMemory mem;
	vk::ImageView view;
};

struct Vertices
{
	vk::Buffer buf;
	vk::DeviceMemory mem;
	vk::PipelineVertexInputStateCreateInfo vi;
	eastl::vector<vk::VertexInputBindingDescription> bindingDescriptions;
	eastl::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

struct Indices
{
	uint count;
	vk::Buffer buf;
	vk::DeviceMemory mem;
};

struct UniformDataVS
{
	vk::Buffer buffer;
	vk::DeviceMemory memory;
	vk::DescriptorBufferInfo descriptor;
};

struct UboVS
{
	glm::mat4 projectionMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
};

struct Pipelines
{
	vk::Pipeline solid;
};

class VKApplication
{
public:

	void initialize();

private:

	bool m_enableValidation = false;
	vk::Instance m_instance = NULL;
	vk::PhysicalDevice m_physDevice = NULL;
	VKSwapchain m_swapchain;
	vk::Device m_device = NULL;
	vk::PhysicalDeviceMemoryProperties m_physDeviceMemoryProperties;
	vk::CommandPool m_commandPool = NULL;
	vk::CommandBuffer m_setupCommandBuffer = NULL;
	eastl::vector<vk::CommandBuffer> m_drawCommandBuffers;
	vk::CommandBuffer m_postPresentCommandBuffer = NULL;

	vk::Format m_depthFormat;
	DepthStencil m_depthStencil;
	vk::Format m_colorFormat = vk::Format::eB8G8R8A8Unorm;
	vk::RenderPass m_renderPass = NULL;
	eastl::vector<vk::Framebuffer> m_framebuffers;

private:

	Vertices m_vertices;
	Indices m_indices;
	UniformDataVS m_uniformDataVS;
	UboVS m_uboVS;
	Pipelines m_pipelines;

	/*
	vk::Queue m_queue = NULL;
	uint m_activeFBIdx = 0;
	vk::DescriptorPool m_descriptorPool = NULL;
	eastl::vector<vk::ShaderModule> m_shaderModules;
	vk::PipelineCache m_pipelineCache = NULL;

	bool m_prepared = false;
	*/
};