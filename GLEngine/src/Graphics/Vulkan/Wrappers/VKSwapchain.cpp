#include "Graphics/Vulkan/Wrappers/VKSwapchain.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "Graphics/Vulkan/Wrappers/VKInstance.h"
#include "Graphics/Vulkan/Wrappers/VKPhysicalDevice.h"
#include "Graphics/Vulkan/Wrappers/VKCommandBuffer.h"

VKSwapchain::~VKSwapchain()
{
	if (m_initialized)
		cleanup();
}

void VKSwapchain::initialize(VKInstance& a_instance, VKPhysicalDevice& a_physDevice)
{
	m_instance = &a_instance;
	m_physDevice = &a_physDevice;

	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.hinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.hwnd(scast<HWND>(GLEngine::graphics->getHWND()));

	m_surface = a_instance.getVKInstance().createWin32SurfaceKHR(win32SurfaceCreateInfo, NULL);

	const eastl::vector<vk::QueueFamilyProperties>& queueFamilyProperties = a_physDevice.getQueueFamilyProperties();
	eastl::vector<vk::Bool32> supportsPresent;
	supportsPresent.resize(queueFamilyProperties.size());
	for (uint i = 0; i < queueFamilyProperties.size(); ++i)
		supportsPresent[i] = a_physDevice.getVKPhysicalDevice().getSurfaceSupportKHR(i, m_surface);

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

	assert(graphicsQueueNodeIndex == a_physDevice.getQueueNodeIndex(VKDevice::EDeviceType::Graphics));

	m_graphicsQueueNodeIndex = graphicsQueueNodeIndex;
	m_presentQueueNodeIndex = presentQueueNodeIndex;

	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats;
	VKVerifier result = a_physDevice.getVKPhysicalDevice().getSurfaceFormatsKHR(m_surface, surfaceFormats);

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

void VKSwapchain::setup(VKCommandBuffer& a_setupCommandBuffer, uint a_width, uint a_height)
{
	assert(m_initialized);
	assert(!m_setup);

	vk::PhysicalDevice physDevice = m_physDevice->getVKPhysicalDevice();
	vk::SwapchainKHR oldSwapchain = m_swapchain;

	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	VKVerifier result = physDevice.getSurfaceCapabilitiesKHR(m_surface, surfaceCapabilities);

	eastl::vector<vk::PresentModeKHR> presentModes;
	result = physDevice.getSurfacePresentModesKHR(m_surface, presentModes);

	vk::Extent2D swapchainExtent(a_width, a_height);
	if (surfaceCapabilities.currentExtent().width() != -1)
	{
		swapchainExtent = surfaceCapabilities.currentExtent();
		assert(a_width == swapchainExtent.width() && a_height == swapchainExtent.height()); // handle resolution change
		/* a_width = swapchainExtent.width();
		a_height = swapchainExtent.height(); */
	}

	vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifoKHR;
	for (uint i = 0; i < presentModes.size(); ++i)
	{
		if (presentModes[i] == vk::PresentModeKHR::eMailboxKHR)
		{
			swapchainPresentMode = vk::PresentModeKHR::eMailboxKHR;
			break;
		}
		if ((swapchainPresentMode != vk::PresentModeKHR::eMailboxKHR) && presentModes[i] == vk::PresentModeKHR::eImmediateKHR)
			swapchainPresentMode = vk::PresentModeKHR::eImmediateKHR;
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

	vk::Device device = m_physDevice->getDevice(VKDevice::EDeviceType::Graphics).getVKDevice();
	m_swapchain = device.createSwapchainKHR(swapchainCreateInfo, NULL);

	if (oldSwapchain)
		device.destroySwapchainKHR(oldSwapchain, NULL);

	result = device.getSwapchainImagesKHR(m_swapchain, m_images);
	m_views.resize(m_images.size());

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

		VKUtils::setImageLayout(a_setupCommandBuffer.getVKCommandBuffer(), m_images[i], vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		colorAttachmentViewCreateInfo.image(m_images[i]);
		m_views[i] = device.createImageView(colorAttachmentViewCreateInfo, NULL);
	}

	m_setup = true;
}

void VKSwapchain::cleanup()
{
	assert(m_initialized);
	assert(false); //TODO

	m_instance = NULL;
	m_physDevice = NULL;

	m_initialized = false;
}
