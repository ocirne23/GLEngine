#include "Graphics/Vulkan/Wrappers/VKDepthStencil.h"

#include "Graphics/Vulkan/Wrappers/VKDevice.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"

VKDepthStencil::~VKDepthStencil()
{
	if (m_initialized)
		cleanup();
}

void VKDepthStencil::initialize(VKDevice& a_device, VKCommandBuffer& a_setupCommandBuffer, uint a_width, uint a_height)
{
	if (m_initialized)
		cleanup();

	VKPhysicalDevice* physDev = a_device.getPhysDevice();
	vk::Format depthFormat = physDev->getDepthFormat();

	vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.imageType(vk::ImageType::e2D)
		.format(depthFormat)
		.extent(vk::Extent3D(a_width, a_height, 1))
		.mipLevels(1)
		.arrayLayers(1)
		.samples(vk::SampleCountFlagBits::e1)
		.tiling(vk::ImageTiling::eOptimal)
		.usage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);

	VKVerifier result = vk::createImage(a_device.getVKDevice(), &imageCreateInfo, NULL, &m_image);

	vk::MemoryRequirements memoryRequirements;
	vk::getImageMemoryRequirements(a_device.getVKDevice(), m_image, memoryRequirements);

	uint memoryTypeIndex = VKUtils::getMemoryType(physDev->getMemProperties(), memoryRequirements.memoryTypeBits(), vk::MemoryPropertyFlagBits::eDeviceLocal);
	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
		.allocationSize(memoryRequirements.size())
		.memoryTypeIndex(memoryTypeIndex);

	result = vk::allocateMemory(a_device.getVKDevice(), &memoryAllocateInfo, NULL, &m_mem);
	result = vk::bindImageMemory(a_device.getVKDevice(), m_image, m_mem, 0);
	VKUtils::setImageLayout(a_setupCommandBuffer.getVKCommandBuffer(), m_image, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::ImageViewCreateInfo depthStencilViewCreateInfo = vk::ImageViewCreateInfo()
		.viewType(vk::ImageViewType::e2D)
		.format(depthFormat)
		.subresourceRange(vk::ImageSubresourceRange()
			.aspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
			.baseMipLevel(0)
			.levelCount(1)
			.baseArrayLayer(0)
			.layerCount(1))
		.image(m_image);

	result = vk::createImageView(a_device.getVKDevice(), &depthStencilViewCreateInfo, NULL, &m_view);

	m_initialized = true;
}

void VKDepthStencil::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO
	m_initialized = false;
}
