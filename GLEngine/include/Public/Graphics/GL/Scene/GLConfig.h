#pragma once

#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "EASTL/string.h"

class GLConfig
{
public:

	enum class ETextures {
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
	enum class EUBOs {
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
	enum class EVBOs {
		GLMeshVertex,
		GLMeshIndice,
		NUM_VBOS
	};

public:

	static void initialize();
	static const eastl::vector<eastl::string>& getGlobalShaderDefines();

	static uint getTextureBindingPoint(ETextures bindingPoint);
	static GLConstantBuffer::Config getUBOConfig(EUBOs ubo);
	static GLVertexBuffer::Config getVBOConfig(EVBOs vbo);
	static uint getMaxLights();
	static uint getMaxMaterials();
	static GLFramebuffer::EMultiSampleType getMultisampleType();

	static void setMultisampleType(GLFramebuffer::EMultiSampleType multisampleType);

private:

	static void initializeDefines();

private:

	static eastl::vector<eastl::string> defines;

	static uint textureBindingPoints[uint(ETextures::NUM_BINDING_POINTS)];
	static GLConstantBuffer::Config uboConfigs[uint(EUBOs::NUM_UBOS)];
	static GLVertexBuffer::Config vboConfigs[uint(EVBOs::NUM_VBOS)];

	static uint maxMaterials;
	static uint maxLights;
	static GLFramebuffer::EMultiSampleType multisampleType;

private:

	GLConfig() {}
};
