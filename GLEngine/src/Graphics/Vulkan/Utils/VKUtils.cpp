#include "Graphics\Vulkan\Utils\VKUtils.h"

#include "Utils/FileHandle.h"

vk::ShaderModule VKUtils::loadShaderModule(const char* a_fileName, vk::Device a_device)
{
	FileHandle file(a_fileName);
	assert(file.exists());
	uint64 size = file.getFileSize();
	eastl::vector<char> data;
	data.reserve(size);
	file.readBytes(&data[0], size, 0);

	vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(size)
		.setPCode(rcast<const uint32_t*>(&data[0]));
	return a_device.createShaderModule(moduleCreateInfo, NULL);
}

/*
* Vulkan examples debug wrapper
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

uint VKUtils::getMemoryType(vk::PhysicalDeviceMemoryProperties a_physDevMemProps, uint a_typeBits, vk::MemoryPropertyFlagBits a_properties)
{
	for (uint i = 0; i < 32; ++i)
	{
		if ((a_typeBits & 1) == 1)
		{
			if ((a_physDevMemProps.memoryTypes[i].propertyFlags & a_properties) == a_properties)
				return i;
		}
		a_typeBits >>= 1;
	}
	assert(false);
	return UINT32_MAX;
}

void VKUtils::setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout)
{
	// Create an image barrier object
	vk::ImageMemoryBarrier imageMemoryBarrier = vk::ImageMemoryBarrier()
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setOldLayout(oldImageLayout)
		.setNewLayout(newImageLayout)
		.setImage(image)
		.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(aspectMask)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setLayerCount(1));

	// Source layouts (old)

	// Undefined layout
	// Only allowed as initial layout!
	// Make sure any writes to the image have been finished
	if (oldImageLayout == vk::ImageLayout::eUndefined)
	{
		//imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
	}

	// Old layout is color attachment
	// Make sure any writes to the color buffer have been finished
	if (oldImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
	}

	// Old layout is transfer source
	// Make sure any reads from the image have been finished
	if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// Old layout is shader read (sampler, input attachment)
	// Make sure any shader reads from the image have been finished
	if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eShaderRead);
	}

	// Target layouts (new)

	// New layout is transfer destination (copy, blit)
	// Make sure any copyies to the image have been finished
	if (newImageLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
	}

	// New layout is transfer source (copy, blit)
	// Make sure any reads from and writes to the image have been finished
	if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.setSrcAccessMask(imageMemoryBarrier.srcAccessMask | vk::AccessFlagBits::eTransferRead);
		imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// New layout is color attachment
	// Make sure any writes to the color buffer hav been finished
	if (newImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
		imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// New layout is depth attachment
	// Make sure any writes to depth/stencil buffer have been finished
	if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		imageMemoryBarrier.setDstAccessMask(imageMemoryBarrier.dstAccessMask | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
	}

	// New layout is shader read (sampler, input attachment)
	// Make sure any writes to the image have been finished
	if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
		imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
	}

	// Put barrier on top
	vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
	vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

	cmdbuffer.pipelineBarrier(srcStageFlags,
		destStageFlags,
		vk::DependencyFlags(),
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}
