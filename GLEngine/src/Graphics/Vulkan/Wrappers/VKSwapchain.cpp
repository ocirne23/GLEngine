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

void VKSwapchain::initialize(VKPhysicalDevice& a_physDevice, uint a_width, uint a_height)
{
	if (m_initialized)
		cleanup();

	m_instance = a_physDevice.getVKInstance();
	m_physDevice = a_physDevice.getVKPhysicalDevice();
	m_device = a_physDevice.getDevice().getVKDevice();

	vk::Win32SurfaceCreateInfoKHR win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.hinstance(scast<HINSTANCE>(GLEngine::graphics->getHINSTANCE()))
		.hwnd(scast<HWND>(GLEngine::graphics->getHWND()));

	m_surface = m_instance.createWin32SurfaceKHR(win32SurfaceCreateInfo, NULL);

	const uint graphicsQueueFamilyIndex = a_physDevice.getQueueFamilyIndex(VKDevice::EDeviceType::Graphics);
	const bool supportsPresent = m_physDevice.getSurfaceSupportKHR(graphicsQueueFamilyIndex, m_surface);
	assert(supportsPresent);

	eastl::vector<vk::SurfaceFormatKHR> surfaceFormats;
	VKVerifier result = m_physDevice.getSurfaceFormatsKHR(m_surface, surfaceFormats);

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



	//


	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	VKVerifier result = m_physDevice.getSurfaceCapabilitiesKHR(m_surface, surfaceCapabilities);

	eastl::vector<vk::PresentModeKHR> presentModes;
	result = m_physDevice.getSurfacePresentModesKHR(m_surface, presentModes);

	vk::Extent2D swapchainExtent(a_width, a_height);
	if (surfaceCapabilities.currentExtent().width() != -1)
	{
		swapchainExtent = surfaceCapabilities.currentExtent();
		assert(a_width == swapchainExtent.width() && a_height == swapchainExtent.height()); // TODO: handle resolution change
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
		.oldSwapchain(NULL)
		.clipped(VK_TRUE)
		.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

	m_swapchain = m_device.createSwapchainKHR(swapchainCreateInfo, NULL);

	result = m_device.getSwapchainImagesKHR(m_swapchain, m_images);
	m_views.resize(m_images.size());

	m_setupCommandBuffer.initialize(a_physDevice.getDevice());
	m_setupCommandBuffer.begin();

	for (uint i = 0; i < m_images.size(); ++i)
	{
		vk::ImageViewCreateInfo colorAttachmentViewCreateInfo = vk::ImageViewCreateInfo()
			.format(m_colorFormat)
			.components(vk::ComponentMapping()
				.r(vk::ComponentSwizzle::eR)
				.g(vk::ComponentSwizzle::eG)
				.b(vk::ComponentSwizzle::eB)
				.a(vk::ComponentSwizzle::eA))
			.subresourceRange(vk::ImageSubresourceRange()
				.aspectMask(vk::ImageAspectFlagBits::eColor)
				.baseMipLevel(0)
				.levelCount(1)
				.baseArrayLayer(0)
				.layerCount(1))				
			.viewType(vk::ImageViewType::e2D);

		VKUtils::setImageLayout(m_setupCommandBuffer.getVKCommandBuffer(), m_images[i], vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		colorAttachmentViewCreateInfo.image(m_images[i]);
		m_views[i] = m_device.createImageView(colorAttachmentViewCreateInfo, NULL);
	}

	m_setupCommandBuffer.end();
	m_setupCommandBuffer.submit();
	m_setupCommandBuffer.waitIdle();

	m_initialized = true;
}

void VKSwapchain::cleanup()
{
	assert(m_initialized);

	assert(false); //TODO

	m_initialized = false;
}

void VKSwapchain::acquireNextImage(vk::Semaphore a_presentCompleteSemaphore, uint& a_currentBuffer)
{
	VKVerifier result = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, a_presentCompleteSemaphore, NULL, a_currentBuffer);
}
