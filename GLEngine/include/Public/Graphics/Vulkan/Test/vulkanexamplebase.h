/*
* Vulkan Example base class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else 
// todo : split linux xcb/x11 and android
#include <xcb/xcb.h>
#endif

#include <iostream>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <string>
#include <array>

#include "vulkan/vulkan.h"
#include "Graphics/Vulkan/vk_cpp.h"

#include "Graphics/Vulkan/Test/vulkantools.h"
#include "Graphics/Vulkan/Test/vulkandebug.h"

#include "Graphics/Vulkan/Test/vulkanswapchain.hpp"
#include "Graphics/Vulkan/Test/vulkanTextureLoader.hpp"
#include "Graphics/Vulkan/Test/vulkanMeshLoader.hpp"

#include "Graphics/Vulkan/Wrappers/VKInstance.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"


#define M_PI 3.14159265359
#define deg_to_rad(deg) deg * float(M_PI / 180)

class VulkanExampleBase
{
public:

	VulkanExampleBase(bool enableValidation);
	~VulkanExampleBase();

	void initVulkan(bool enableValidation);
	void setupConsole(std::string title);
	HWND setupWindow(HINSTANCE hinstance, HWND hwnd);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void render();
	void viewChanged();

	VkBool32 getMemoryType(uint32_t typeBits, VkFlags properties, uint32_t *typeIndex);
	void createCommandPool();
	void setupDepthStencil();
	void setupFrameBuffer();
	void setupRenderPass();

	void initSwapchain();
	void setupSwapChain();

	bool checkCommandBuffers();
	void createCommandBuffers();
	void destroyCommandBuffers();
	void createSetupCommandBuffer();
	void flushSetupCommandBuffer();
	void createPipelineCache();
	void prepare();

	VkPipelineShaderStageCreateInfo loadShader(const char* fileName, VkShaderStageFlagBits stage);
	VkPipelineShaderStageCreateInfo loadShaderGLSL(const char* fileName, VkShaderStageFlagBits stage);
	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory);
	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkDescriptorBufferInfo *descriptor);
	void loadMesh(
		const char *filename,
		vkMeshLoader::MeshBuffer *meshBuffer,
		std::vector<vkMeshLoader::VertexLayout> vertexLayout,
		float scale);
	void renderLoop();
	void submitPostPresentBarrier(VkImage image);

private:

	VkResult createInstance(bool enableValidation);
	VkResult createDevice(VkDeviceQueueCreateInfo requestedQueues, bool enableValidation);

	void buildCommandBuffers();
	void draw();
	void prepareVertices();
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void preparePipelines();
	void prepareUniformBuffers();
	void updateUniformBuffers();

public:

	bool prepared = false;
	uint32_t width = 1280;
	uint32_t height = 720;
	VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };
	float zoom = 0;
	float timer = 0.0f;
	float timerSpeed = 0.25f;
	bool paused = false;
	float rotationSpeed = 1.0f;
	float zoomSpeed = 1.0f;
	glm::vec3 rotation = glm::vec3();
	glm::vec2 mousePos;
	std::string title = "Vulkan Example";
	std::string name = "vulkanExample";
	struct
	{
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthStencil;
	HWND window;
	HINSTANCE windowInstance;

protected:

	VKInstance m_instance;
	VKPhysicalDevice* m_physDevice = NULL;
	VKDevice* m_device = NULL;
	VKSwapchain* m_swapchain = NULL;


	float frameTimer = 1.0f;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	VkDevice device;
	VkQueue queue;
	VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormat depthFormat;
	VkCommandPool cmdPool;
	VkCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;
	VkCommandBuffer postPresentCmdBuffer = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCmdBuffers;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> frameBuffers;
	uint32_t currentBuffer = 0;
	VkDescriptorPool descriptorPool;
	std::vector<VkShaderModule> shaderModules;
	VkPipelineCache pipelineCache;
	VulkanSwapChain swapChain;
	vkTools::VulkanTextureLoader *textureLoader = nullptr;

private:

	bool enableValidation = false;

	struct {
		VkBuffer buf;
		VkDeviceMemory mem;
		VkPipelineVertexInputStateCreateInfo vi;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		int count;
		VkBuffer buf;
		VkDeviceMemory mem;
	} indices;

	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor;
	}  uniformDataVS;

	struct {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uboVS;

	struct {
		VkPipeline solid;
	} pipelines;

	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;
};

