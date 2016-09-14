/*
* Vulkan Example - Texture loading (and display) example (including mip maps)
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include "vulkanExampleBase.h"

// Vertex layout for this example
struct Vertex {
    float pos[3];
    float uv[2];
    float normal[3];
};

class VulkanExample : public vkx::ExampleBase {
    using Parent = ExampleBase;
public:
    // Contains all Vulkan objects that are required to store and use a texture
    // Note that this repository contains a texture loader (TextureLoader.h)
    // that encapsulates texture loading functionality in a class that is used
    // in subsequent demos
    CreateImageResult texture;

    struct {
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        vk::PipelineVertexInputStateCreateInfo inputState;
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    } vertices;

    struct {
        int count;
        vk::Buffer buffer;
        vk::DeviceMemory memory;
    } indices;

    vkx::UniformData uniformDataVS;

    struct UboVS {
        glm::mat4 projection;
        glm::mat4 model;
        glm::vec4 viewPos;
        float lodBias = 0.0f;
    } uboVS;

    struct {
        vk::Pipeline solid;
    } pipelines;

    vk::PipelineLayout pipelineLayout;
    vk::DescriptorSet descriptorSet;
    vk::DescriptorSetLayout descriptorSetLayout;

	VulkanExample() : vkx::ExampleBase(ENABLE_VALIDATION) {
        camera.setZoom(-2.5f);
        camera.setRotation({ 0.0f, 15.0f, 0.0f });
        title = "Vulkan Example - Texturing";
        enableTextOverlay = true;
    }

    ~VulkanExample() {
        // Clean up used Vulkan resources 
        // Note : Inherited destructor cleans up resources stored in base class

        // Clean up texture resources
        texture.destroy();

        device.destroyPipeline(pipelines.solid);

        device.destroyPipelineLayout(pipelineLayout);
        device.destroyDescriptorSetLayout(descriptorSetLayout);

        device.destroyBuffer(vertices.buffer);
        device.freeMemory(vertices.memory);

        device.destroyBuffer(indices.buffer);
        device.freeMemory(indices.memory);

        device.destroyBuffer(uniformDataVS.buffer);
        device.freeMemory(uniformDataVS.memory);
    }

    // Create an image memory barrier for changing the layout of
    // an image and put it into an active command buffer
    void setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t mipLevel, uint32_t mipLevelCount) {
        // Create an image barrier object
        vk::ImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
        imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevel;
        imageMemoryBarrier.subresourceRange.levelCount = mipLevelCount;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        // Only sets masks for layouts used in this example
        // For a more complete version that can be used with
        // other layouts see vkx::setImageLayout

        // Source layouts (new)

        if (oldImageLayout == vk::ImageLayout::ePreinitialized) {
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        } else if (oldImageLayout == vk::ImageLayout::eTransferDstOptimal) {
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        } else if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal) {
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        } else if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        }

        // Target layouts (new)

        if (newImageLayout == vk::ImageLayout::eTransferDstOptimal) {
            // New layout is transfer destination (copy, blit)
            // Make sure any reads from and writes to the image have been finished
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        } else if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            // New layout is shader read (sampler, input attachment)
            // Make sure any writes to the image have been finished
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        } else if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal) {
            // New layout is transfer source (copy, blit)
            // Make sure any reads from and writes to the image have been finished
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        }

        // Put barrier on top
        vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
        vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

        // Put barrier inside setup command buffer
        cmdBuffer.pipelineBarrier(srcStageFlags, destStageFlags, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
    }

    void loadTexture(std::string fileName, vk::Format format, bool forceLinearTiling) {
#if defined(__ANDROID__)
        // Textures are stored inside the apk on Android (compressed)
        // So they need to be loaded via the asset manager
        AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, fileName.c_str(), AASSET_MODE_STREAMING);
        assert(asset);
        size_t size = AAsset_getLength(asset);
        assert(size > 0);

        void *textureData = malloc(size);
        AAsset_read(asset, textureData, size);
        AAsset_close(asset);

        gli::texture2D tex2D(gli::load((const char*)textureData, size));
#else
        gli::texture2D tex2D(gli::load(fileName));
#endif

        assert(!tex2D.empty());
        auto dimensions = tex2D.dimensions();

        vk::FormatProperties formatProperties;

        // Get device properites for the requested texture format
        formatProperties = physicalDevice.getFormatProperties(format);

        // Only use linear tiling if requested (and supported by the device)
        // Support for linear tiling is mostly limited, so prefer to use
        // optimal tiling instead
        // On most implementations linear tiling will only support a very
        // limited amount of formats and features (mip maps, cubemaps, arrays, etc.)
        vk::Bool32 useStaging = true;

        // Only use linear tiling if forced
        if (forceLinearTiling) {
            // Don't use linear if format is not supported for (linear) shader sampling
            useStaging = !(formatProperties.linearTilingFeatures &  vk::FormatFeatureFlagBits::eSampledImage);
        }

        // Load mip map level 0 to linear tiling image
        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = format;
        imageCreateInfo.mipLevels = tex2D.levels();
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
        imageCreateInfo.extent = vk::Extent3D{ (uint32_t)tex2D[0].dimensions().x, (uint32_t)tex2D[0].dimensions().y, 1 };

        if (useStaging) {
            // Create optimal tiled target image
            imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
            imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
            texture = stageToDeviceImage(imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, tex2D);
        } else {
            // Prefer using optimal tiling, as linear tiling  may support only a small set of features 
            // depending on implementation (e.g. no mip maps, only one layer, etc.)
            imageCreateInfo.tiling = vk::ImageTiling::eLinear;
            imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;
            texture = createImage(imageCreateInfo, vk::MemoryPropertyFlagBits::eHostVisible);

            // Get sub resource layout
            // Mip map count, array layer, etc.
            vk::ImageSubresource subRes;
            subRes.aspectMask = vk::ImageAspectFlagBits::eColor;

            // Get sub resources layout 
            // Includes row pitch, size offsets, etc.
            vk::SubresourceLayout subResLayout = device.getImageSubresourceLayout(texture.image, subRes);

            // Map image memory
            texture.map();
            // Copy image data into memory
            texture.copy(tex2D[subRes.mipLevel].size(), tex2D[subRes.mipLevel].data());
            texture.unmap();

            // Linear tiled images don't need to be staged
            // and can be directly used as textures
            withPrimaryCommandBuffer([&](const vk::CommandBuffer& cmdBuffer){
                // Setup image memory barrier transfer image to shader read layout
                setImageLayout(
                    cmdBuffer,
                    texture.image,
                    vk::ImageAspectFlagBits::eColor,
                    vk::ImageLayout::ePreinitialized,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    0,
                    1);
            });
        }

        // Create sampler
        // In Vulkan textures are accessed by samplers
        // This separates all the sampling information from the 
        // texture data
        // This means you could have multiple sampler objects
        // for the same texture with different settings
        // Similar to the samplers available with OpenGL 3.3
        vk::SamplerCreateInfo sampler;
        sampler.magFilter = vk::Filter::eLinear;
        sampler.minFilter = vk::Filter::eLinear;
        sampler.mipmapMode = vk::SamplerMipmapMode::eLinear;
        sampler.addressModeU = vk::SamplerAddressMode::eRepeat;
        sampler.addressModeV = vk::SamplerAddressMode::eRepeat;
        sampler.addressModeW = vk::SamplerAddressMode::eRepeat;
        sampler.mipLodBias = 0.0f;
        sampler.compareOp = vk::CompareOp::eNever;
        sampler.minLod = 0.0f;
        // Max level-of-detail should match mip level count
        sampler.maxLod = (useStaging) ? (float)tex2D.levels() : 0.0f;
        // Enable anisotropic filtering
        sampler.maxAnisotropy = 8;
        sampler.anisotropyEnable = VK_TRUE;
        sampler.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        texture.sampler = device.createSampler(sampler);

        // Create image view
        // Textures are not directly accessed by the shaders and
        // are abstracted by image views containing additional
        // information and sub resource ranges
        vk::ImageViewCreateInfo view;
        view.image;
        view.viewType = vk::ImageViewType::e2D;
        view.format = format;
        view.components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };
        view.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        view.subresourceRange.baseMipLevel = 0;
        view.subresourceRange.baseArrayLayer = 0;
        view.subresourceRange.layerCount = 1;
        // Linear tiling usually won't support mip maps
        // Only set mip map count if optimal tiling is used
        view.subresourceRange.levelCount = (useStaging) ? tex2D.levels() : 1;
        view.image = texture.image;
        texture.view = device.createImageView(view);
    }

    void updateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) {
        cmdBuffer.setViewport(0, vkx::viewport(size));
        cmdBuffer.setScissor(0, vkx::rect2D(size));
        cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.solid);
        vk::DeviceSize offsets = 0;
        cmdBuffer.bindVertexBuffers(VERTEX_BUFFER_BIND_ID, vertices.buffer, offsets);
        cmdBuffer.bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);

        cmdBuffer.drawIndexed(indices.count, 1, 0, 0, 0);
    }

    void generateQuad() {
        // Setup vertices for a single uv-mapped quad
#define DIM 1.0f
#define NORMAL { 0.0f, 0.0f, 1.0f }
        std::vector<Vertex> vertexBuffer =
        {
            { {  DIM,  DIM, 0.0f }, { 1.0f, 1.0f }, NORMAL },
            { { -DIM,  DIM, 0.0f }, { 0.0f, 1.0f }, NORMAL },
            { { -DIM, -DIM, 0.0f }, { 0.0f, 0.0f }, NORMAL },
            { {  DIM, -DIM, 0.0f }, { 1.0f, 0.0f }, NORMAL }
        };
#undef dim
#undef normal
        auto result = createBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vertexBuffer);
        vertices.buffer = result.buffer;
        vertices.memory = result.memory;

        // Setup indices
        std::vector<uint32_t> indexBuffer = { 0,1,2, 2,3,0 };
        indices.count = indexBuffer.size();
        result = createBuffer(vk::BufferUsageFlagBits::eIndexBuffer, indexBuffer);
        indices.buffer = result.buffer;
        indices.memory = result.memory;
    }

    void setupVertexDescriptions() {
        // Binding description
        vertices.bindingDescriptions.resize(1);
        vertices.bindingDescriptions[0] =
            vkx::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, sizeof(Vertex), vk::VertexInputRate::eVertex);

        // Attribute descriptions
        // Describes memory layout and shader positions
        vertices.attributeDescriptions.resize(3);
        // Location 0 : Position
        vertices.attributeDescriptions[0] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 0, vk::Format::eR32G32B32Sfloat, 0);
        // Location 1 : Texture coordinates
        vertices.attributeDescriptions[1] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 1, vk::Format::eR32G32Sfloat, sizeof(float) * 3);
        // Location 1 : Vertex normal
        vertices.attributeDescriptions[2] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 2, vk::Format::eR32G32B32Sfloat, sizeof(float) * 5);

        vertices.inputState = vk::PipelineVertexInputStateCreateInfo();
        vertices.inputState.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
        vertices.inputState.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
        vertices.inputState.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
        vertices.inputState.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
    }

    void setupDescriptorPool() {
        // Example uses one ubo and one image sampler
        std::vector<vk::DescriptorPoolSize> poolSizes =
        {
            vkx::descriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
            vkx::descriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1)
        };

        vk::DescriptorPoolCreateInfo descriptorPoolInfo =
            vkx::descriptorPoolCreateInfo(poolSizes.size(), poolSizes.data(), 2);

        descriptorPool = device.createDescriptorPool(descriptorPoolInfo);
    }

    void setupDescriptorSetLayout() {
        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings =
        {
            // Binding 0 : Vertex shader uniform buffer
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eUniformBuffer,
                vk::ShaderStageFlagBits::eVertex,
                0),
            // Binding 1 : Fragment shader image sampler
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eCombinedImageSampler,
                vk::ShaderStageFlagBits::eFragment,
                1)
        };

        vk::DescriptorSetLayoutCreateInfo descriptorLayout =
            vkx::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), setLayoutBindings.size());

        descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout);

        vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
            vkx::pipelineLayoutCreateInfo(&descriptorSetLayout, 1);

        pipelineLayout = device.createPipelineLayout(pPipelineLayoutCreateInfo);
    }

    void setupDescriptorSet() {
        vk::DescriptorSetAllocateInfo allocInfo =
            vkx::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);

        descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

        // vk::Image descriptor for the color map texture
        vk::DescriptorImageInfo texDescriptor =
            vkx::descriptorImageInfo(texture.sampler, texture.view, vk::ImageLayout::eGeneral);

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets =
        {
            // Binding 0 : Vertex shader uniform buffer
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eUniformBuffer,
                0,
                &uniformDataVS.descriptor),
            // Binding 1 : Fragment shader texture sampler
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                &texDescriptor)
        };

        device.updateDescriptorSets(writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
    }

    void preparePipelines() {
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState =
            vkx::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, vk::PipelineInputAssemblyStateCreateFlags(), VK_FALSE);

        vk::PipelineRasterizationStateCreateInfo rasterizationState =
            vkx::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

        vk::PipelineColorBlendAttachmentState blendAttachmentState =
            vkx::pipelineColorBlendAttachmentState();

        vk::PipelineColorBlendStateCreateInfo colorBlendState =
            vkx::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

        vk::PipelineDepthStencilStateCreateInfo depthStencilState =
            vkx::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual);

        vk::PipelineViewportStateCreateInfo viewportState =
            vkx::pipelineViewportStateCreateInfo(1, 1);

        vk::PipelineMultisampleStateCreateInfo multisampleState =
            vkx::pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

        std::vector<vk::DynamicState> dynamicStateEnables = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynamicState =
            vkx::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), dynamicStateEnables.size());

        // Load shaders
        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

        shaderStages[0] = loadShader(getAssetPath() + "shaders/texture/texture.vert.spv", vk::ShaderStageFlagBits::eVertex);
        shaderStages[1] = loadShader(getAssetPath() + "shaders/texture/texture.frag.spv", vk::ShaderStageFlagBits::eFragment);

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo =
            vkx::pipelineCreateInfo(pipelineLayout, renderPass);

        pipelineCreateInfo.pVertexInputState = &vertices.inputState;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.stageCount = shaderStages.size();
        pipelineCreateInfo.pStages = shaderStages.data();

        trashPipeline(pipelines.solid);
        pipelines.solid = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];
    }

    // Prepare and initialize uniform buffer containing shader uniforms
    void prepareUniformBuffers() {
        // Vertex shader uniform buffer block
        uniformDataVS = createUniformBuffer(uboVS);
        updateUniformBuffers();
    }

    void updateUniformBuffers() {
        // Vertex shader
        uboVS.projection = camera.matrices.perspective;
        glm::mat4 viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, camera.position.z));
        uboVS.model = viewMatrix * glm::translate(glm::mat4(), glm::vec3(camera.position.x, camera.position.y, 0));
        uboVS.model = uboVS.model * glm::mat4_cast(camera.orientation);
        uboVS.viewPos = glm::vec4(0.0f, 0.0f, -camera.position.z, 0.0f);
        uniformDataVS.copy(uboVS);
    }

    void prepare() {
        Parent::prepare();
        generateQuad();
        setupVertexDescriptions();
        prepareUniformBuffers();
        loadTexture(
            getAssetPath() + "textures/pattern_02_bc2.ktx",
            vk::Format::eBc2UnormBlock,
            false);
        setupDescriptorSetLayout();
        preparePipelines();
        setupDescriptorPool();
        setupDescriptorSet();
        updateDrawCommandBuffers();
        prepared = true;
    }

    void viewChanged() override {
        updateUniformBuffers();
    }

    void changeLodBias(float delta) {
        uboVS.lodBias += delta;
        if (uboVS.lodBias < 0.0f) {
            uboVS.lodBias = 0.0f;
        }
        if (uboVS.lodBias > 8.0f) {
            uboVS.lodBias = 8.0f;
        }
        updateUniformBuffers();
    }

    void keyPressed(uint32_t a_keyCode) override {
        switch (EKey(a_keyCode)) {
		case EKey::KP_PLUS:
            changeLodBias(0.1f);
            break;
		case EKey::KP_MINUS:
            changeLodBias(-0.1f);
            break;
        }
    }
};

RUN_EXAMPLE(VulkanExample)
