#include "Graphics/Vulkan/VKApplication.h"

#include "EASTL/vector.h"
#include "EASTL/array.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vulkan/Utils/VKVerifier.h"
#include "Graphics/Vulkan/Utils/VKDebug.h"
#include "Graphics/Vulkan/Utils/VKUtils.h"
#include "gsl/gsl.h"

#define VERTEX_BUFFER_BIND_ID 0

BEGIN_UNNAMED_NAMESPACE()

vk::Instance createInstance(bool a_enableValidation)
{
	eastl::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.apiVersion(VK_API_VERSION)
		.engineVersion(0)
		.pEngineName("GLEngine")
		.applicationVersion(0)
		.pApplicationName("GLApp");

	vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
		.pApplicationInfo(&appInfo);

	if (enabledExtensions.size())
	{
		if (a_enableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		instanceCreateInfo.enabledExtensionCount((uint32_t)enabledExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames(enabledExtensions.data());
	}
	if (a_enableValidation)
	{
		instanceCreateInfo.enabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT); // todo : change validation layer names!
		instanceCreateInfo.ppEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	vk::Instance instance;
	VKVerifier result = vk::createInstance(&instanceCreateInfo, NULL, &instance);
	return instance;
}

uint findGraphicsQueueIndex(vk::PhysicalDevice a_physDevice)
{
	eastl::vector<vk::QueueFamilyProperties> queueFamilyProperties = a_physDevice.getQueueFamilyProperties();;
	for (uint graphicsQueueIndex = 0; graphicsQueueIndex < queueFamilyProperties.size(); graphicsQueueIndex++)
	{
		if (queueFamilyProperties[graphicsQueueIndex].queueFlags() & vk::QueueFlagBits::eGraphics)
			return graphicsQueueIndex;
	}
	assert(false);
	return UINT32_MAX;
}

vk::Device createDevice(vk::PhysicalDevice a_physDevice, uint a_graphicsQueueIndex, bool a_enableValidation)
{
	eastl::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	eastl::array<float, 1> queuePriorities = { 0.0f };

	vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
		.queueFamilyIndex(a_graphicsQueueIndex)
		.queueCount(1)
		.pQueuePriorities(queuePriorities.data());

	vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
		.queueCreateInfoCount(1)
		.pQueueCreateInfos(&deviceQueueCreateInfo);

	if (enabledExtensions.size())
	{
		deviceCreateInfo.enabledExtensionCount(uint(enabledExtensions.size()));
		deviceCreateInfo.ppEnabledExtensionNames(enabledExtensions.data());
	}
	if (a_enableValidation)
	{
		deviceCreateInfo.enabledLayerCount(VKDebug::VALIDATION_LAYER_COUNT);
		deviceCreateInfo.ppEnabledLayerNames(VKDebug::VALIDATION_LAYER_NAMES);
	}

	vk::Device device;
	VKVerifier result = a_physDevice.createDevice(&deviceCreateInfo, nullptr, &device);
	return device;
}

vk::PhysicalDevice findPhysicalDevice(vk::Instance a_instance)
{
	eastl::vector<vk::PhysicalDevice> physDevices;
	VKVerifier result = a_instance.enumeratePhysicalDevices(physDevices);
	if (physDevices.empty())
	{
		print("No Vulkan compatible devices found\n");
		return VK_NULL_HANDLE;
	}
	else
	{
		print("Num Vulkan compatible devices: %i\n", physDevices.size());
	}
	return physDevices[0];
}

uint getMemoryType(vk::PhysicalDeviceMemoryProperties a_physDevMemProps, uint a_typeBits, vk::MemoryPropertyFlagBits a_properties)
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

vk::Format getDepthFormat(vk::PhysicalDevice a_physDevice)
{
	// Find supported depth format
	// We prefer 24 bits of depth and 8 bits of stencil, but that may not be supported by all implementations
	eastl::vector<vk::Format> depthFormats = { vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };
	for (auto& format : depthFormats)
	{
		vk::FormatProperties formatProperties;
		a_physDevice.getFormatProperties(format, &formatProperties);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProperties.optimalTilingFeatures() & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
			return format;
	}
	assert(false);
	return vk::Format::eUndefined;
}

DepthStencil createDepthStencil(vk::PhysicalDeviceMemoryProperties a_physDeviceMemoryProperties, vk::Device a_device, vk::CommandBuffer a_setupCommandBuffer, vk::Format a_depthFormat, uint a_width, uint a_height)
{
	DepthStencil depthStencil;

	vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.imageType(vk::ImageType::e2D)
		.format(a_depthFormat)
		.extent(vk::Extent3D(a_width, a_height, 1))
		.mipLevels(1)
		.arrayLayers(1)
		.samples(vk::SampleCountFlagBits::e1)
		.tiling(vk::ImageTiling::eOptimal)
		.usage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);

	VKVerifier result = a_device.createImage(&imageCreateInfo, NULL, &depthStencil.image);

	vk::MemoryRequirements memoryRequirements;
	a_device.getImageMemoryRequirements(depthStencil.image, &memoryRequirements);
	
	uint memoryTypeIndex = getMemoryType(a_physDeviceMemoryProperties, memoryRequirements.memoryTypeBits(), vk::MemoryPropertyFlagBits::eDeviceLocal);
	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
		.allocationSize(memoryRequirements.size())
		.memoryTypeIndex(memoryTypeIndex);

	depthStencil.mem = a_device.allocateMemory(memoryAllocateInfo, NULL);
	result = vk::allocateMemory(a_device, &memoryAllocateInfo, NULL, &depthStencil.mem);
	result = vk::bindImageMemory(a_device, depthStencil.image, depthStencil.mem, 0);
	VKUtils::setImageLayout(a_setupCommandBuffer, depthStencil.image, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	
	vk::ImageViewCreateInfo depthStencilViewCreateInfo = vk::ImageViewCreateInfo()
		.viewType(vk::ImageViewType::e2D)
		.format(a_depthFormat)
		.subresourceRange(vk::ImageSubresourceRange()
			.aspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
			.baseMipLevel(0)
			.levelCount(1)
			.baseArrayLayer(0)
			.layerCount(1))
		.image(depthStencil.image);

	result = vk::createImageView(a_device, &depthStencilViewCreateInfo, NULL, &depthStencil.view);

	return depthStencil;
}

eastl::vector<vk::Framebuffer> createFramebuffers(vk::Device a_device, VKSwapchain& a_swapChain, DepthStencil a_depthStencil, vk::RenderPass a_renderPass, uint a_width, uint a_height)
{
	vk::ImageView attachments[2];
	attachments[1] = a_depthStencil.view;

	vk::FramebufferCreateInfo framebufferCreateInfo = vk::FramebufferCreateInfo()
		.renderPass(a_renderPass)
		.attachmentCount(2)
		.pAttachments(attachments)
		.width(a_width)
		.height(a_height)
		.layers(1);

	eastl::vector<vk::Framebuffer> framebuffers;
	/*framebuffers.resize(a_swapChain.getNumImages());
	for (uint i = 0; i < a_swapChain.getNumImages(); ++i)
	{
		attachments[0] = a_swapChain.getBuffer(i).view;
		VKVerifier result = vk::createFramebuffer(a_device, &framebufferCreateInfo, NULL, &framebuffers[i]);
	}*/
	return framebuffers;
}

vk::CommandPool createCommandPool(vk::Device a_device, uint a_graphicsQueueIndex)
{
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.queueFamilyIndex(a_graphicsQueueIndex)
		.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	vk::CommandPool pool;
	VKVerifier result = vk::createCommandPool(a_device, &commandPoolCreateInfo, NULL, &pool);
	return pool;
}

void freeCommandBuffer(vk::Device a_device, vk::CommandPool a_pool, vk::CommandBuffer a_commandBuffer)
{
	if (a_commandBuffer != VK_NULL_HANDLE)
	{
		vk::freeCommandBuffers(a_device, a_pool, 1, &a_commandBuffer);
	}
}

eastl::vector<vk::CommandBuffer> createCommandBuffers(vk::Device a_device, vk::CommandPool a_commandPool, uint a_count)
{
	eastl::vector<vk::CommandBuffer> buffers;
	buffers.resize(a_count);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.commandPool(a_commandPool)
		.level(vk::CommandBufferLevel::ePrimary)
		.commandBufferCount(a_count);

	VKVerifier result = vk::allocateCommandBuffers(a_device, &commandBufferAllocateInfo, buffers.data());

	return buffers;
}

vk::RenderPass createRenderPass(vk::Device a_device, vk::Format a_colorFormat, vk::Format a_depthFormat)
{
	vk::AttachmentDescription attachments[2];
	attachments[0].format(a_colorFormat);
	attachments[0].samples(vk::SampleCountFlagBits::e1);
	attachments[0].loadOp(vk::AttachmentLoadOp::eClear);
	attachments[0].storeOp(vk::AttachmentStoreOp::eStore);
	attachments[0].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	attachments[0].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[0].initialLayout(vk::ImageLayout::eColorAttachmentOptimal);
	attachments[0].finalLayout(vk::ImageLayout::eColorAttachmentOptimal);

	attachments[1].format(a_depthFormat);
	attachments[1].samples(vk::SampleCountFlagBits::e1);
	attachments[1].loadOp(vk::AttachmentLoadOp::eClear);
	attachments[1].storeOp(vk::AttachmentStoreOp::eStore);
	attachments[1].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	attachments[1].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	attachments[1].initialLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
	attachments[1].finalLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);

	vk::AttachmentReference colorReference = vk::AttachmentReference()
		.attachment(0)
		.layout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentReference depthReference = vk::AttachmentReference()
		.attachment(1)
		.layout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass = vk::SubpassDescription()
		.pipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.flags(0)
		.inputAttachmentCount(0)
		.pInputAttachments(NULL)
		.colorAttachmentCount(1)
		.pColorAttachments(&colorReference)
		.pResolveAttachments(NULL)
		.pDepthStencilAttachment(&depthReference)
		.preserveAttachmentCount(0)
		.pPreserveAttachments(NULL);

	vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
		.attachmentCount(2)
		.pAttachments(attachments)
		.subpassCount(1)
		.pSubpasses(&subpass)
		.dependencyCount(0)
		.pDependencies(NULL);

	vk::RenderPass renderPass;
	VKVerifier result = vk::createRenderPass(a_device, &renderPassCreateInfo, NULL, &renderPass);
	return renderPass;
}

END_UNNAMED_NAMESPACE()

BEGIN_UNNAMED_NAMESPACE()

struct Buffer
{
	vk::Device device       = VK_NULL_HANDLE;
	vk::Buffer buffer       = VK_NULL_HANDLE;
	vk::DeviceMemory memory = VK_NULL_HANDLE;
	uint64 allocationSize   = 0;
};

Buffer createBuffer(vk::Device a_device, 
                        vk::PhysicalDeviceMemoryProperties a_physMemProps, 
                        vk::BufferUsageFlagBits a_usageFlags, 
                        vk::MemoryPropertyFlagBits a_propertyFlags, 
                        uint64 a_byteSize)
{
	Buffer buffer;
	buffer.device = a_device;

	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.size(a_byteSize)
		.usage(a_usageFlags);

	VKVerifier result = vk::createBuffer(a_device, &bufferCreateInfo, NULL, &buffer.buffer);
	
	vk::MemoryRequirements memReqs;
	vk::getBufferMemoryRequirements(a_device, buffer.buffer, memReqs);

	uint memoryTypeIndex = getMemoryType(a_physMemProps, memReqs.memoryTypeBits(), a_propertyFlags);
	vk::MemoryAllocateInfo memAlloc = vk::MemoryAllocateInfo()
		.allocationSize(memReqs.size())
		.memoryTypeIndex(memoryTypeIndex);
	result = vk::allocateMemory(a_device, &memAlloc, NULL, &buffer.memory);

	return buffer;
}

void* mapBuffer(Buffer& a_buffer)
{
	void* data = NULL;
	VKVerifier result = vk::mapMemory(a_buffer.device, a_buffer.memory, 0, a_buffer.allocationSize, 0, &data);
	return data;
}

void unmapBuffer(Buffer& a_buffer)
{
	vk::unmapMemory(a_buffer.device, a_buffer.memory);
	VKVerifier result = vk::bindBufferMemory(a_buffer.device, a_buffer.buffer, a_buffer.memory, 0);
}

Vertices prepareVertices(vk::Device a_device, vk::PhysicalDeviceMemoryProperties a_physMemProps)
{
	Vertices vertices;
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};
	eastl::vector<Vertex> vertexBufferData = {
		{ glm::vec3(1, 1, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(-1, 1, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3(0, -1, 0), glm::vec3(0, 0, 1) }
	};
	uint64 vertexBufferByteSize = vertexBufferData.size_bytes();

	Buffer vertexBuffer = createBuffer(a_device, 
	                                   a_physMemProps, 
	                                   vk::BufferUsageFlagBits::eVertexBuffer, 
	                                   vk::MemoryPropertyFlagBits::eHostVisible, 
	                                   vertexBufferByteSize);

	void* data = mapBuffer(vertexBuffer);
	memcpy(data, vertexBufferData.data(), vertexBufferByteSize);
	unmapBuffer(vertexBuffer);

	vertices.buf = vertexBuffer.buffer;
	vertices.mem = vertexBuffer.memory;

	// Binding description
	vertices.bindingDescriptions = {
		vk::VertexInputBindingDescription()
			.binding(VERTEX_BUFFER_BIND_ID)
			.stride(sizeof(Vertex))
			.inputRate(vk::VertexInputRate::eVertex)
	};

	// Attribute descriptions
	// Describes memory layout and shader attribute locations
	vertices.attributeDescriptions = {
		vk::VertexInputAttributeDescription() // Position
			.binding(VERTEX_BUFFER_BIND_ID)
			.location(0)
			.format(vk::Format::eR32G32B32Sint)
			.offset(0),
		vk::VertexInputAttributeDescription() // Color
			.binding(VERTEX_BUFFER_BIND_ID) 
			.location(1)
			.format(vk::Format::eR32G32B32Sint)
			.offset(sizeof(glm::vec3))
	};

	vertices.vi = vk::PipelineVertexInputStateCreateInfo()
		.vertexBindingDescriptionCount(uint(vertices.bindingDescriptions.size()))
		.pVertexBindingDescriptions(vertices.bindingDescriptions.data())
		.vertexAttributeDescriptionCount(uint(vertices.attributeDescriptions.size()))
		.pVertexAttributeDescriptions(vertices.attributeDescriptions.data());

	return vertices;
}

Indices prepareIndices(vk::Device a_device, vk::PhysicalDeviceMemoryProperties a_physMemProps)
{
	Indices indices;

	eastl::vector<uint> indexBufferData = { 0, 1, 2 };
	uint64 indexBufferByteSize = indexBufferData.size_bytes();

	Buffer indiceBuffer = createBuffer(a_device,
		a_physMemProps,
		vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible,
		indexBufferByteSize);

	void* data = mapBuffer(indiceBuffer);
	memcpy(data, indexBufferData.data(), indexBufferByteSize);
	unmapBuffer(indiceBuffer);

	indices.count = uint(indexBufferData.size());
	indices.buf = indiceBuffer.buffer;
	indices.mem = indiceBuffer.memory;

	return indices;
}

UniformDataVS prepareUniformBuffers(vk::Device a_device, vk::PhysicalDeviceMemoryProperties a_physMemProps)
{
	Buffer ubo = createBuffer(a_device, 
		a_physMemProps,
		vk::BufferUsageFlagBits::eUniformBuffer, 
		vk::MemoryPropertyFlagBits::eHostVisible, sizeof(UboVS));

	return UniformDataVS();
}

END_UNNAMED_NAMESPACE()

void VKApplication::initialize()
{
	const bool enableValidation = true;

	m_instance = createInstance(enableValidation);
	m_physDevice = findPhysicalDevice(m_instance);
	//m_swapchain.initialize(m_instance, m_physDevice);
	//const uint graphicsQueueIndex = m_swapchain.getGraphicsQueueIndex();
	//m_device = createDevice(m_physDevice, graphicsQueueIndex, enableValidation);
	vk::getPhysicalDeviceMemoryProperties(m_physDevice, m_physDeviceMemoryProperties);
	//m_commandPool = createCommandPool(m_device, graphicsQueueIndex);
	if (m_setupCommandBuffer != VK_NULL_HANDLE) freeCommandBuffer(m_device, m_commandPool, m_setupCommandBuffer);
	m_setupCommandBuffer = createCommandBuffers(m_device, m_commandPool, 1)[0];
	vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();
	VKVerifier result = vk::beginCommandBuffer(m_setupCommandBuffer, commandBufferBeginInfo);

	uint width = GLEngine::graphics->getWindowWidth();
	uint height = GLEngine::graphics->getWindowHeight();
	//m_swapchain.setup(m_device, m_setupCommandBuffer, width, height);

	//m_drawCommandBuffers = createCommandBuffers(m_device, m_commandPool, m_swapchain.getNumImages());
	m_postPresentCommandBuffer = createCommandBuffers(m_device, m_commandPool, 1)[0];
	m_depthFormat = getDepthFormat(m_physDevice);
	m_depthStencil = createDepthStencil(m_physDeviceMemoryProperties, m_device, m_setupCommandBuffer, m_depthFormat, width, height);
	m_renderPass = createRenderPass(m_device, m_colorFormat, m_depthFormat);
	m_framebuffers = createFramebuffers(m_device, m_swapchain, m_depthStencil, m_renderPass, width, height);

	m_vertices = prepareVertices(m_device, m_physDeviceMemoryProperties);
	m_indices = prepareIndices(m_device, m_physDeviceMemoryProperties);
	/*
	prepareVertices();
	prepareUniformBuffers();
	setupDescriptorSetLayout();
	preparePipelines();
	setupDescriptorPool();
	setupDescriptorSet();
	buildCommandBuffers();
	*/
	print("end\n");
}