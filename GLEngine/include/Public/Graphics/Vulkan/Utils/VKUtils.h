#pragma once

#include "Core.h"

#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKUtils
{
public:

	static vk::ShaderModule loadShaderModule(const char* fileName, vk::Device device);
	static void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);
	static uint getMemoryType(vk::PhysicalDeviceMemoryProperties physDevMemProps, uint typeBits, vk::MemoryPropertyFlagBits properties);
};