#include "Graphics/Vulkan/Wrappers/VKSwapchainTMP.h"
#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/Wrappers/VKInstance.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"

void VKSwapchain::initialize(VKInstance& a_instance, VKPhysicalDevice& a_physDevice)
{
	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.hinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.hwnd(scast<HWND>(GLEngine::graphics->getHWND()));

	VKVerifier result = vk::createWin32SurfaceKHR(a_instance.getVKInstance(), &win32SurfaceCreateInfo, NULL, &m_surface);

	eastl::vector<vk::QueueFamilyProperties>& queueFamilyProperties = a_physDevice.getQueueFamilyProperties();
	eastl::vector<vk::Bool32> supportsPresent;
	supportsPresent.resize(queueFamilyProperties.size());
	for (uint i = 0; i < queueFamilyProperties.size(); ++i)
		vk::getPhysicalDeviceSurfaceSupportKHR(a_physDevice.getVKPhysicalDevice(), i, m_surface, supportsPresent[i]);

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint graphicsQueueNodeIndex = UINT32_MAX;
	uint presentQueueNodeIndex = UINT32_MAX;
	for (uint i = 0; i < queueFamilyProperties.size(); i++)
	{
		if ((queueFamilyProperties[i].queueFlags() & vk::QueueFlagBits::eGraphics) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX)
			{
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX)
	{
		// If there's no queue that supports both present and graphics
		// try to find a separate present queue
		for (uint i = 0; i < queueFamilyProperties.size(); ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		assert(false);
	if (graphicsQueueNodeIndex != presentQueueNodeIndex)
		assert(false);

	m_graphicsQueueNodeIndex = graphicsQueueNodeIndex;
	m_presentQueueNodeIndex = presentQueueNodeIndex;

	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats;
	result = vk::getPhysicalDeviceSurfaceFormatsKHR(a_physDevice.getVKPhysicalDevice(), m_surface, surfaceFormats);

	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format() == vk::Format::eUndefined)
	{
		m_colorFormat = vk::Format::eB8G8R8A8Unorm;;
	}
	else
	{
		assert(surfaceFormats.size() >= 1);
		m_colorFormat = surfaceFormats[0].format();
	}
	m_colorSpace = surfaceFormats[0].colorSpace();

	m_initialized = true;
}

void VKSwapchainTMP::initialize(vk::Instance a_instance, vk::PhysicalDevice a_physDevice)
{
	m_instance = a_instance;
	m_physDevice = a_physDevice;

	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.hinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.hwnd(scast<HWND>(GLEngine::graphics->getHWND()));

	VKVerifier result = vk::createWin32SurfaceKHR(a_instance, &win32SurfaceCreateInfo, NULL, &m_surface);
	
	eastl::vector<vk::QueueFamilyProperties> queueFamilyProperties = vk::getPhysicalDeviceQueueFamilyProperties(m_physDevice);
	eastl::vector<vk::Bool32> supportsPresent;
	supportsPresent.resize(queueFamilyProperties.size());
	for (uint i = 0; i < queueFamilyProperties.size(); ++i)
		vk::getPhysicalDeviceSurfaceSupportKHR(m_physDevice, i, m_surface, supportsPresent[i]);

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint graphicsQueueNodeIndex = UINT32_MAX;
	uint presentQueueNodeIndex = UINT32_MAX;
	for (uint i = 0; i < queueFamilyProperties.size(); i++)
	{
		if ((queueFamilyProperties[i].queueFlags() & vk::QueueFlagBits::eGraphics) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX)
			{
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX)
	{
		// If there's no queue that supports both present and graphics
		// try to find a separate present queue
		for (uint i = 0; i < queueFamilyProperties.size(); ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		assert(false);
	if (graphicsQueueNodeIndex != presentQueueNodeIndex)
		assert(false);
	
	m_graphicsQueueIndex = graphicsQueueNodeIndex;

	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats;
	result = vk::getPhysicalDeviceSurfaceFormatsKHR(m_physDevice, m_surface, surfaceFormats);

	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format() == vk::Format::eUndefined)
	{
		m_colorFormat = vk::Format::eB8G8R8A8Unorm;;
	}
	else
	{
		assert(surfaceFormats.size() >= 1);
		m_colorFormat = surfaceFormats[0].format();
	}
	m_colorSpace = surfaceFormats[0].colorSpace();
}

void VKSwapchainTMP::setup(vk::Device a_device, vk::CommandBuffer a_commandBuffer, uint& a_width, uint& a_height)
{
	m_device = a_device;
	vk::SwapchainKHR oldSwapchain = m_swapchain;

	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	VKVerifier result = vk::getPhysicalDeviceSurfaceCapabilitiesKHR(m_physDevice, m_surface, surfaceCapabilities);

	eastl::vector<vk::PresentModeKHR> presentModes;
	result = vk::getPhysicalDeviceSurfacePresentModesKHR(m_physDevice, m_surface, presentModes);

	vk::Extent2D swapchainExtent(a_width, a_height);
	if (surfaceCapabilities.currentExtent().width() != -1)
	{
		swapchainExtent = surfaceCapabilities.currentExtent();
		assert(a_width == swapchainExtent.width() && a_height == swapchainExtent.height()); // handle resolution change
		a_width = swapchainExtent.width();
		a_height = swapchainExtent.height();
	}

	vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeFifoKhr;
	for (uint i = 0; i < presentModes.size(); ++i)
	{
		if (presentModes[i] == vk::PresentModeKHR::eVkPresentModeMailboxKhr)
		{
			swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeMailboxKhr;
			break;
		}
		if ((swapchainPresentMode != vk::PresentModeKHR::eVkPresentModeMailboxKhr) && presentModes[i] == vk::PresentModeKHR::eVkPresentModeImmediateKhr)
			swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeImmediateKhr;
	}

	uint desiredNumSwapchainImages = surfaceCapabilities.minImageCount() + 1;
	if ((surfaceCapabilities.maxImageCount() > 0) && (desiredNumSwapchainImages > surfaceCapabilities.maxImageCount()))
		desiredNumSwapchainImages = surfaceCapabilities.maxImageCount();

	vk::SurfaceTransformFlagBitsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms() & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	else
		preTransform = surfaceCapabilities.currentTransform();

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
		.surface(m_surface)
		.minImageCount(desiredNumSwapchainImages)
		.imageFormat(m_colorFormat)
		.imageColorSpace(m_colorSpace)
		.imageExtent(swapchainExtent)
		.imageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.preTransform(preTransform)
		.imageArrayLayers(1)
		.imageSharingMode(vk::SharingMode::eExclusive)
		.queueFamilyIndexCount(0)
		.pQueueFamilyIndices(NULL)
		.presentMode(swapchainPresentMode)
		.oldSwapchain(oldSwapchain)
		.clipped(VK_TRUE)
		.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

	result = vk::createSwapchainKHR(m_device, &swapchainCreateInfo, NULL, &m_swapchain);

	if (oldSwapchain != VK_NULL_HANDLE)
		vk::destroySwapchainKHR(m_device, oldSwapchain, NULL);

	result = vk::getSwapchainImagesKHR(m_device, m_swapchain, m_images);
	m_buffers.resize(m_images.size());

	for (uint i = 0; i < m_images.size(); ++i)
	{
		vk::ImageViewCreateInfo colorAttachmentViewCreateInfo = vk::ImageViewCreateInfo()
			.format(m_colorFormat)
			.components(vk::ComponentMapping(
				vk::ComponentSwizzle::eR,  // r
				vk::ComponentSwizzle::eG,  // g
				vk::ComponentSwizzle::eB,  // b
				vk::ComponentSwizzle::eA)) // a
			.subresourceRange(vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor, // aspectMask
				0,	// baseMipLevel
				1,  // levelCount
				0,  // baseArrayLayer
				1)) // layerCount
			.viewType(vk::ImageViewType::e2D);

		m_buffers[i].image = m_images[i];
		VKUtils::setImageLayout(a_commandBuffer, m_buffers[i].image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKhr);
		colorAttachmentViewCreateInfo.image(m_buffers[i].image);
		result = vk::createImageView(a_device, &colorAttachmentViewCreateInfo, NULL, &m_buffers[i].view);
	}
}