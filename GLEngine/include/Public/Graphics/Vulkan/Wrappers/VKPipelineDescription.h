#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKPipelineDescription
{
public:

	eastl::string m_vertexShaderPath;
	eastl::string m_fragmentShaderPath;

	vk::DescriptorBufferInfo m_bufferInfo;
	vk::PipelineVertexInputStateCreateInfo m_vertexInputState;
};