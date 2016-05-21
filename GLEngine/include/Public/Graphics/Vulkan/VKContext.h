#pragma once

#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKPipelineDescription.h"

class VKContext
{
public:


	static void test();



private:

	static void initialize();
	static void setupPipeline(VKPipelineDescription& pipelineDescription);
	static void buildCommandBuffers(VKBuffer & vertexBuffer, VKBuffer & indexBuffer, uint numIndices, vk::IndexType indexType);
	static void render();

public:

	static vk::Instance instance;

	static vk::PhysicalDevice physDevice;
	static vk::PhysicalDeviceProperties physDeviceProperties;
	static vk::PhysicalDeviceMemoryProperties physDeviceMemoryProperties;
	static vk::PhysicalDeviceFeatures physDeviceFeatures;

	static vk::Device device;
	static vk::Queue queue;
	static vk::CommandPool commandPool;
	static vk::Format depthFormat;
	static vk::Format colorFormat;
	static vk::ColorSpaceKHR colorSpace;

	static vk::SurfaceKHR surface;
	static vk::SwapchainKHR swapchain;
	static eastl::vector<vk::Image> swapchainImages;
	static eastl::vector<vk::ImageView> swapchainImageViews;

	static vk::Semaphore presentCompleteSemaphore;
	static vk::Semaphore renderCompleteSemaphore;
	static vk::Semaphore textOverlayCompleteSemaphore;

	static vk::SubmitInfo submitInfo;

	static eastl::vector<vk::CommandBuffer> drawCommandBuffers;
	static vk::CommandBuffer prePresentCommandBuffer;
	static vk::CommandBuffer postPresentCommandBuffer;

	static vk::Image depthStencilImage;
	static vk::DeviceMemory depthStencilMemory;
	static vk::ImageView depthStencilImageView;

	static vk::PipelineCache pipelineCache;

	static vk::RenderPass renderPass;
	static eastl::vector<vk::Framebuffer> framebuffers;
	static uint currentBuffer;

	static vk::PipelineLayout pipelineLayout;

	static vk::Pipeline pipeline;
	static vk::DescriptorPool descriptorPool;
	static vk::DescriptorSet descriptorSet;
};