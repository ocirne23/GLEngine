#pragma once

#include "Core.h"
#include "Graphics/Vulkan/vk_cpp.h"
#include "Graphics/Vulkan/Wrappers/VKBuffer.h"

class VKPipelineDescription
{
public:

	eastl::vector<vk::VertexInputBindingDescription> bindingDescriptions;
	eastl::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
	struct Descriptor
	{
		VKBuffer* buffer;
		vk::DescriptorType type;
		vk::ShaderStageFlagBits stage;
		uint size;
		uint offset;
		uint binding;
	};
	eastl::vector<Descriptor> descriptors;

	eastl::string vertexShaderPath   = "Shaders/Vulkan/model.vert.spv";
	eastl::string fragmentShaderPath = "Shaders/Vulkan/model.frag.spv";

	////

	bool depthTestEnabled      = true;
	bool depthWriteEnabled     = true;
	bool depthBoundTestEnabled = false;
	bool stencilTestEnabled    = false;
	bool blendingEnabled       = false;
	float lineWidth            = 1.0f;
	vk::ColorComponentFlags colorWriteMask =
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;
	vk::CompareOp depthCompareOp         = vk::CompareOp::eLessOrEqual;
	vk::CullModeFlags cullFace           = vk::CullModeFlagBits::eBack;
	vk::FrontFace frontFace              = vk::FrontFace::eClockwise;
	vk::PrimitiveTopology topology       = vk::PrimitiveTopology::eTriangleList;
	vk::PolygonMode polygonMode          = vk::PolygonMode::eFill;
	vk::SampleCountFlagBits multisamples = vk::SampleCountFlagBits::e1;
	vk::IndexType indexType              = vk::IndexType::eUint32;
};