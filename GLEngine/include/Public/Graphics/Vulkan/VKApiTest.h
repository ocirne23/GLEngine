#pragma once

#include "Core.h"

#include "Graphics/Vulkan/Wrappers/VKBuffer.h"
#include "Graphics/Vulkan/Wrappers/VKInstance.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"
#include "Graphics/Vulkan/Wrappers/VKRenderPass.h"
#include "Graphics/Vulkan/Wrappers/VKPipeline.h"
#include "Graphics/Vulkan/Wrappers/VKFramebuffer.h"

#include <glm/glm.hpp>

class VKApiTest
{
public:

	struct UboData
	{
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	};
	struct Vertex 
	{
		Vertex(glm::vec3 a_pos, glm::vec3 a_col) : pos(a_pos), col(a_col) {}
		glm::vec3 pos;
		glm::vec3 col;
	};

public:

	void test();

private:

	VKInstance m_instance;
	VKPhysicalDevice m_physDevice;
	VKDevice* m_device;
	VKSwapchain m_swapchain;
	VKRenderPass m_renderPass;
	VKPipeline m_pipeline;
	uint m_currentBufferIdx = 0;

	eastl::vector<VKFramebuffer> m_framebuffers;

	VKCommandBuffer m_setupCommandBuffer;
	eastl::vector<VKCommandBuffer> m_drawCommandBuffers;
	VKCommandBuffer m_postPresentCommandBuffer;

	VKBuffer m_uniformBuffer;
	VKBuffer m_vertexBuffer;
	VKBuffer m_indiceBuffer;
};