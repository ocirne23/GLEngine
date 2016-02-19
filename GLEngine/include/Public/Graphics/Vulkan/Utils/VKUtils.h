#pragma once

#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/vk_cpp.h"

class VKUtils
{
public:

	static void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);

};