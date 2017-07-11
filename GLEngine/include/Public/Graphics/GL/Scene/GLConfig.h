#pragma once

#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

class GLConfig
{
public:

	enum class ETextures 
	{
		DiffuseAtlasArray,
		NormalAtlasArray,
		MetalnessAtlasArray,
		RoughnessAtlasArray,
		OpacityAtlasArray,
		DFVTexture,
		ClusteredLightGrid,
		ClusteredLightIndice,
		SunShadow,
		Color,
		Depth,
		HBAONoise,
		Blur,
		HBAOResult,
		BloomResult,
		NUM_BINDING_POINTS
	};
	enum class EUBOs 
	{
		ModelData,
		CameraVars,
		LightingGlobals,
		MaterialProperties,
		ClusteredLightPositionRange,
		ClusteredLightColorIntensities,
		ClusteredGlobals,
		HBAOGlobals,
		SettingsGlobals,
		NUM_UBOS
	};
	enum class EVBOs 
	{
		GLMeshVertex,
		GLMeshIndice,
		NUM_VBOS
	};

public:

	static void initialize();
	static const eastl::vector<eastl::string>& getGlobalShaderDefines();
	/** Reinitializes all the shader definitions, shaders should be reloaded */
	static void setMultisampleType(GLTexture::EMultiSampleType multisampleType);

	static uint getTextureBindingPoint(ETextures bindingPoint);
	static GLConstantBuffer::Config getUBOConfig(EUBOs ubo);
	static GLVertexBuffer::Config getVBOConfig(EVBOs vbo);
	static uint getMaxLights();
	static uint getMaxMaterials();
	static GLTexture::EMultiSampleType getMultisampleType();
	static void setupFramebufferTextures();
	static uint getHBAOResolutionScale();
	static glm::ivec2 getSunShadowMapRes();

public:

	struct RenderTargets
	{
		GLTexture sceneColor;
		GLTexture sceneDepth;
		GLTexture sunShadow;
		GLTexture bloom;
		GLTexture hbao;
		GLTexture downsampleDepth;
		GLTexture fxaa;
	};

	static RenderTargets rt;

private:

	static void initializeShaderDefines();

private:

	static eastl::vector<eastl::string> defines;

	static uint textureBindingPoints[uint(ETextures::NUM_BINDING_POINTS)];
	static GLConstantBuffer::Config uboConfigs[uint(EUBOs::NUM_UBOS)];
	static GLVertexBuffer::Config vboConfigs[uint(EVBOs::NUM_VBOS)];

	static uint maxMaterials;
	static uint maxLights;
	static GLTexture::EMultiSampleType multisampleType;
	static uint hbaoResolutionScale;
	static glm::ivec2 sunShadowMapResolution;

private:

	GLConfig() {}
};
