/*
* Vulkan Example - Displacement mapping with tessellation shaders
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "vulkanExampleBase.h"


// Vertex layout for this example
std::vector<vkx::VertexLayout> vertexLayout =
{
    vkx::VertexLayout::VERTEX_LAYOUT_POSITION,
    vkx::VertexLayout::VERTEX_LAYOUT_NORMAL,
    vkx::VertexLayout::VERTEX_LAYOUT_UV
};

class VulkanExample : public vkx::ExampleBase {
private:
    struct {
        vkx::Texture colorMap;
        vkx::Texture heightMap;
    } textures;
public:
    bool splitScreen = true;

    struct {
        vk::PipelineVertexInputStateCreateInfo inputState;
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    } vertices;

    struct {
        vkx::MeshBuffer object;
    } meshes;

    vkx::UniformData uniformDataTC, uniformDataTE;

    struct UboTC {
        float tessLevel = 8.0;
    } uboTC;

    struct UboTE {
        glm::mat4 projection;
        glm::mat4 model;
        glm::vec4 lightPos = glm::vec4(0.0, -25.0, 0.0, 0.0);
        float tessAlpha = 1.0;
        float tessStrength = 1.0;
    } uboTE;

    struct {
        vk::Pipeline solid;
        vk::Pipeline wire;
        vk::Pipeline solidPassThrough;
        vk::Pipeline wirePassThrough;
    } pipelines;
    vk::Pipeline *pipelineLeft = &pipelines.solidPassThrough;
    vk::Pipeline *pipelineRight = &pipelines.solid;

    vk::PipelineLayout pipelineLayout;
    vk::DescriptorSet descriptorSet;
    vk::DescriptorSetLayout descriptorSetLayout;

    VulkanExample() : vkx::ExampleBase(ENABLE_VALIDATION) {
        camera.setZoom(-35);
        camera.setRotation({ -35.0, 0.0, 0 });
        title = "Vulkan Example - Tessellation shader displacement mapping";
        // Support for tessellation shaders is optional, so check first
        if (!deviceFeatures.tessellationShader) {
            throw std::runtime_error("Selected GPU does not support tessellation shaders!");
        }
    }

    ~VulkanExample() {
        // Clean up used Vulkan resources 
        // Note : Inherited destructor cleans up resources stored in base class
        device.destroyPipeline(pipelines.solid);
        device.destroyPipeline(pipelines.wire);
        device.destroyPipeline(pipelines.solidPassThrough);
        device.destroyPipeline(pipelines.wirePassThrough);

        device.destroyPipelineLayout(pipelineLayout);
        device.destroyDescriptorSetLayout(descriptorSetLayout);

        meshes.object.destroy();

        device.destroyBuffer(uniformDataTC.buffer);
        device.freeMemory(uniformDataTC.memory);

        device.destroyBuffer(uniformDataTE.buffer);
        device.freeMemory(uniformDataTE.memory);

        textures.colorMap.destroy();
        textures.heightMap.destroy();
    }

    void loadTextures() {
        textures.colorMap = textureLoader->loadTexture(
            getAssetPath() + "textures/stonewall_colormap_bc3.dds",
            vk::Format::eBc3UnormBlock);
        textures.heightMap = textureLoader->loadTexture(
            getAssetPath() + "textures/stonewall_heightmap_rgba.dds",
            vk::Format::eR8G8B8A8Unorm);
    }

    void updateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) {
        vk::Viewport viewport = vkx::viewport(splitScreen ? (float)size.width / 2.0f : (float)size.width, (float)size.height, 0.0f, 1.0f);
        cmdBuffer.setViewport(0, viewport);
        cmdBuffer.setScissor(0, vkx::rect2D(size));
        cmdBuffer.setLineWidth(1.0f);
        cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
        cmdBuffer.bindVertexBuffers(VERTEX_BUFFER_BIND_ID, meshes.object.vertices.buffer, { 0 });
        cmdBuffer.bindIndexBuffer(meshes.object.indices.buffer, 0, vk::IndexType::eUint32);

        if (splitScreen) {
            cmdBuffer.setViewport(0, viewport);
            cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelineLeft);
            cmdBuffer.drawIndexed(meshes.object.indexCount, 1, 0, 0, 0);
            viewport.x += viewport.width;
        }

        cmdBuffer.setViewport(0, viewport);
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelineRight);
        cmdBuffer.drawIndexed(meshes.object.indexCount, 1, 0, 0, 0);
    }

    void loadMeshes() {
        meshes.object = loadMesh(getAssetPath() + "models/torus.obj", vertexLayout, 0.25f);
    }

    void setupVertexDescriptions() {
        // Binding description
        vertices.bindingDescriptions.resize(1);
        vertices.bindingDescriptions[0] =
            vkx::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, vkx::vertexSize(vertexLayout), vk::VertexInputRate::eVertex);

        // Attribute descriptions
        // Describes memory layout and shader positions
        vertices.attributeDescriptions.resize(3);

        // Location 0 : Position
        vertices.attributeDescriptions[0] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 0, vk::Format::eR32G32B32Sfloat, 0);

        // Location 1 : Normals
        vertices.attributeDescriptions[1] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 1, vk::Format::eR32G32B32Sfloat, sizeof(float) * 3);

        // Location 2 : Texture coordinates
        vertices.attributeDescriptions[2] =
            vkx::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 2, vk::Format::eR32G32Sfloat, sizeof(float) * 6);

        vertices.inputState = vk::PipelineVertexInputStateCreateInfo();
        vertices.inputState.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
        vertices.inputState.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
        vertices.inputState.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
        vertices.inputState.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
    }

    void setupDescriptorPool() {
        // Example uses two ubos and two image samplers
        std::vector<vk::DescriptorPoolSize> poolSizes =
        {
            vkx::descriptorPoolSize(vk::DescriptorType::eUniformBuffer, 2),
            vkx::descriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 2)
        };

        vk::DescriptorPoolCreateInfo descriptorPoolInfo =
            vkx::descriptorPoolCreateInfo(poolSizes.size(), poolSizes.data(), 2);

        descriptorPool = device.createDescriptorPool(descriptorPoolInfo);
    }

    void setupDescriptorSetLayout() {
        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings =
        {
            // Binding 0 : Tessellation control shader ubo
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eUniformBuffer,
                vk::ShaderStageFlagBits::eTessellationControl,
                0),
            // Binding 1 : Tessellation evaluation shader ubo
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eUniformBuffer,
                vk::ShaderStageFlagBits::eTessellationEvaluation,
                1),
            // Binding 2 : Tessellation evaluation shader displacement map image sampler
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eCombinedImageSampler,
                vk::ShaderStageFlagBits::eTessellationEvaluation,
                2),
            // Binding 3 : Fragment shader color map image sampler
            vkx::descriptorSetLayoutBinding(
                vk::DescriptorType::eCombinedImageSampler,
                vk::ShaderStageFlagBits::eFragment,
                3),
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

        // Displacement map image descriptor
        vk::DescriptorImageInfo texDescriptorDisplacementMap =
            vkx::descriptorImageInfo(textures.heightMap.sampler, textures.heightMap.view, vk::ImageLayout::eGeneral);

        // Color map image descriptor
        vk::DescriptorImageInfo texDescriptorColorMap =
            vkx::descriptorImageInfo(textures.colorMap.sampler, textures.colorMap.view, vk::ImageLayout::eGeneral);

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets =
        {
            // Binding 0 : Tessellation control shader ubo
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eUniformBuffer,
                0,
                &uniformDataTC.descriptor),
            // Binding 1 : Tessellation evaluation shader ubo
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eUniformBuffer,
                1,
                &uniformDataTE.descriptor),
            // Binding 2 : Displacement map
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eCombinedImageSampler,
                2,
                &texDescriptorDisplacementMap),
            // Binding 3 : Color map
            vkx::writeDescriptorSet(
                descriptorSet,
                vk::DescriptorType::eCombinedImageSampler,
                3,
                &texDescriptorColorMap),
        };

        device.updateDescriptorSets(writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
    }

    void preparePipelines() {
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState =
            vkx::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::ePatchList, vk::PipelineInputAssemblyStateCreateFlags(), VK_FALSE);

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
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth
        };
        vk::PipelineDynamicStateCreateInfo dynamicState =
            vkx::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), dynamicStateEnables.size());

        vk::PipelineTessellationStateCreateInfo tessellationState =
            vkx::pipelineTessellationStateCreateInfo(3);

        // Tessellation pipeline
        // Load shaders
        std::array<vk::PipelineShaderStageCreateInfo, 4> shaderStages;
        shaderStages[0] = loadShader(getAssetPath() + "shaders/displacement/base.vert.spv", vk::ShaderStageFlagBits::eVertex);
        shaderStages[1] = loadShader(getAssetPath() + "shaders/displacement/base.frag.spv", vk::ShaderStageFlagBits::eFragment);
        shaderStages[2] = loadShader(getAssetPath() + "shaders/displacement/displacement.tesc.spv", vk::ShaderStageFlagBits::eTessellationControl);
        shaderStages[3] = loadShader(getAssetPath() + "shaders/displacement/displacement.tese.spv", vk::ShaderStageFlagBits::eTessellationEvaluation);

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
        pipelineCreateInfo.pTessellationState = &tessellationState;
        pipelineCreateInfo.stageCount = shaderStages.size();
        pipelineCreateInfo.pStages = shaderStages.data();
        pipelineCreateInfo.renderPass = renderPass;

        // Solid pipeline
        pipelines.solid = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];

        // Wireframe pipeline
        rasterizationState.polygonMode = vk::PolygonMode::eLine;
        pipelines.wire = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];


        // Pass through pipelines
        // Load pass through tessellation shaders (Vert and frag are reused)
        shaderStages[2] = loadShader(getAssetPath() + "shaders/displacement/passthrough.tesc.spv", vk::ShaderStageFlagBits::eTessellationControl);
        shaderStages[3] = loadShader(getAssetPath() + "shaders/displacement/passthrough.tese.spv", vk::ShaderStageFlagBits::eTessellationEvaluation);
        // Solid
        rasterizationState.polygonMode = vk::PolygonMode::eFill;
        pipelines.solidPassThrough = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];

        // Wireframe
        rasterizationState.polygonMode = vk::PolygonMode::eLine;
        pipelines.wirePassThrough = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo, nullptr)[0];

    }

    // Prepare and initialize uniform buffer containing shader uniforms
    void prepareUniformBuffers() {
        // Tessellation evaluation shader uniform buffer
        uniformDataTE = createBuffer(vk::BufferUsageFlagBits::eUniformBuffer, uboTE);
        uniformDataTE.map();

        // Tessellation control shader uniform buffer
        uniformDataTC = createBuffer(vk::BufferUsageFlagBits::eUniformBuffer, uboTC);
        uniformDataTC.map();
        updateUniformBuffers();
    }

    void updateUniformBuffers() {
        // Tessellation eval
        uboTE.projection = glm::perspective(glm::radians(45.0f), (float)(size.width* ((splitScreen) ? 0.5f : 1.0f)) / (float)size.height, 0.1f, 256.0f);
        uboTE.model = camera.matrices.view;
        uniformDataTE.copy(uboTE);

        // Tessellation control
        uniformDataTC.copy(uboTC);
    }

    void prepare() {
        ExampleBase::prepare();
        loadMeshes();
        loadTextures();
        setupVertexDescriptions();
        prepareUniformBuffers();
        setupDescriptorSetLayout();
        preparePipelines();
        setupDescriptorPool();
        setupDescriptorSet();
        updateDrawCommandBuffers();
        prepared = true;
    }

    virtual void render() {
        if (!prepared)
            return;
        draw();
    }

    virtual void viewChanged() {
        updateUniformBuffers();
    }

    void changeTessellationLevel(float delta) {
        uboTC.tessLevel += delta;
        // Clamp
        uboTC.tessLevel = fmax(1.0, fmin(uboTC.tessLevel, 32.0));
        updateUniformBuffers();
    }

    void togglePipelines() {
        if (pipelineRight == &pipelines.solid) {
            pipelineRight = &pipelines.wire;
            pipelineLeft = &pipelines.wirePassThrough;
        } else {
            pipelineRight = &pipelines.solid;
            pipelineLeft = &pipelines.solidPassThrough;
        }
        updateDrawCommandBuffers();
    }

    void toggleSplitScreen() {
        splitScreen = !splitScreen;
        updateDrawCommandBuffers();
        updateUniformBuffers();
    }


    void keyPressed(uint32_t key) override {
        switch (EKey(key)) {
		case EKey::KP_PLUS:
            changeTessellationLevel(0.25);
            break;
        case EKey::KP_MINUS:
            changeTessellationLevel(-0.25);
            break;
		case EKey::W:
            togglePipelines();
            break;
		case EKey::S:
            toggleSplitScreen();
            break;
        }
    }
};

RUN_EXAMPLE(VulkanExample)

