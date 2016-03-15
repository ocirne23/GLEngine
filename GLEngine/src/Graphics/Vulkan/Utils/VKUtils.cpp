#include "Graphics\Vulkan\Utils\VKUtils.h"

uint VKUtils::getMemoryType(vk::PhysicalDeviceMemoryProperties a_physDevMemProps, uint a_typeBits, vk::MemoryPropertyFlagBits a_properties)
{
	for (uint i = 0; i < 32; ++i)
	{
		if ((a_typeBits & 1) == 1)
		{
			if ((a_physDevMemProps.memoryTypes()[i].propertyFlags() & a_properties) == a_properties)
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
		.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.oldLayout(oldImageLayout)
		.newLayout(newImageLayout)
		.image(image)
		.subresourceRange(vk::ImageSubresourceRange()
			.aspectMask(aspectMask)
			.baseMipLevel(0)
			.levelCount(1)
			.layerCount(1));

	// Source layouts (old)

	// Undefined layout
	// Only allowed as initial layout!
	// Make sure any writes to the image have been finished
	if (oldImageLayout == vk::ImageLayout::eUndefined)
	{
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
	}

	// Old layout is color attachment
	// Make sure any writes to the color buffer have been finished
	if (oldImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
	}

	// Old layout is transfer source
	// Make sure any reads from the image have been finished
	if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// Old layout is shader read (sampler, input attachment)
	// Make sure any shader reads from the image have been finished
	if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eShaderRead);
	}

	// Target layouts (new)

	// New layout is transfer destination (copy, blit)
	// Make sure any copyies to the image have been finished
	if (newImageLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		imageMemoryBarrier.dstAccessMask(vk::AccessFlagBits::eTransferWrite);
	}

	// New layout is transfer source (copy, blit)
	// Make sure any reads from and writes to the image have been finished
	if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.srcAccessMask(imageMemoryBarrier.srcAccessMask() | vk::AccessFlagBits::eTransferRead);
		imageMemoryBarrier.dstAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// New layout is color attachment
	// Make sure any writes to the color buffer hav been finished
	if (newImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);
	}

	// New layout is depth attachment
	// Make sure any writes to depth/stencil buffer have been finished
	if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		imageMemoryBarrier.dstAccessMask(imageMemoryBarrier.dstAccessMask() | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
	}

	// New layout is shader read (sampler, input attachment)
	// Make sure any writes to the image have been finished
	if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
		imageMemoryBarrier.dstAccessMask(vk::AccessFlagBits::eShaderRead);
	}

	// Put barrier on top
	vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
	vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

	vk::cmdPipelineBarrier(
		cmdbuffer,
		srcStageFlags, 
		destStageFlags,
		vk::DependencyFlags(),
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}
