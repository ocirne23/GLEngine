#include "Graphics/GL/Scene/GLConfig.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/GL/Tech/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"
#include "Utils/StringUtils.h"

eastl::vector<eastl::string>    GLConfig::defines;
uint                            GLConfig::textureBindingPoints[uint(ETextures::NUM_BINDING_POINTS)];
GLConstantBuffer::Config        GLConfig::uboConfigs[uint(EUBOs::NUM_UBOS)];
GLVertexBuffer::Config          GLConfig::vboConfigs[uint(EVBOs::NUM_VBOS)];
uint                            GLConfig::maxMaterials    = 200;
uint                            GLConfig::maxLights       = 100;
GLFramebuffer::EMultiSampleType GLConfig::multisampleType = GLFramebuffer::EMultiSampleType::NONE;

void GLConfig::initialize()
{
	textureBindingPoints[uint(ETextures::TextureAtlasArray)]    = 1;
	textureBindingPoints[uint(ETextures::DFVTexture)]           = 2;
	textureBindingPoints[uint(ETextures::ClusteredLightGrid)]   = 3;
	textureBindingPoints[uint(ETextures::ClusteredLightIndice)] = 4;
	textureBindingPoints[uint(ETextures::SunShadow)]            = 5;
	textureBindingPoints[uint(ETextures::Color)]                = 1;
	textureBindingPoints[uint(ETextures::Depth)]                = 2;
	textureBindingPoints[uint(ETextures::HBAONoise)]            = 3;
	textureBindingPoints[uint(ETextures::Blur)]                 = 3;
	textureBindingPoints[uint(ETextures::HBAOResult)]           = 4;
	textureBindingPoints[uint(ETextures::BloomResult)]          = 5;

	uboConfigs[uint(EUBOs::ModelMatrix)] =                    { 0, "ModelMatrix",             GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::ModelMatrixData) };
	uboConfigs[uint(EUBOs::CameraVars)] =                     { 1, "CameraVars",              GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::CameraVarsData) };
	uboConfigs[uint(EUBOs::LightingGlobals)] =                { 2, "LightingGlobals",         GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLRenderer::LightingGlobalsData) };
	uboConfigs[uint(EUBOs::MaterialProperties)] =             { 3, "MaterialProperties",      GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLMaterial) * maxMaterials };
	uboConfigs[uint(EUBOs::ClusteredLightPositionRange)] =    { 4, "LightPositionRanges",     GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * maxLights };
	uboConfigs[uint(EUBOs::ClusteredLightColorIntensities)] = { 5, "LightColorIntensities",   GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * maxLights };
	uboConfigs[uint(EUBOs::ClusteredGlobals)] =               { 6, "ClusteredShadingGlobals", GLConstantBuffer::EDrawUsage::STATIC, sizeof(ClusteredShading::GlobalsUBO) };
	uboConfigs[uint(EUBOs::HBAOGlobals)] =                    { 7, "HBAOGlobals",             GLConstantBuffer::EDrawUsage::STATIC, sizeof(HBAO::GlobalsUBO) };
	uboConfigs[uint(EUBOs::SettingsGlobals)] =                { 8, "SettingsGlobals",         GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLRenderer::SettingsGlobalsData) };

	static VertexAttribute GLMESH_VB_ATTRIBS[] = {
		VertexAttribute(0, VertexAttribute::EFormat::FLOAT, 3),       // Position
		VertexAttribute(1, VertexAttribute::EFormat::FLOAT, 2),       // Texcoord
		VertexAttribute(2, VertexAttribute::EFormat::FLOAT, 3),       // Normals
		// VertexAttribute(3, VertexAttribute::EFormat::FLOAT, 3),       // Tangents
		// VertexAttribute(4, VertexAttribute::EFormat::FLOAT, 3),       // Bitangents
		VertexAttribute(3, VertexAttribute::EFormat::UNSIGNED_INT, 1) // MaterialID
	};
	vboConfigs[uint(EVBOs::GLMeshVertex)] = {
		GLVertexBuffer::EBufferType::ARRAY,
		GLVertexBuffer::EDrawUsage::STATIC,
		eastl::vector<VertexAttribute>(GLMESH_VB_ATTRIBS, GLMESH_VB_ATTRIBS + ARRAY_SIZE(GLMESH_VB_ATTRIBS))
	};
	vboConfigs[uint(EVBOs::GLMeshIndice)] = {
		GLVertexBuffer::EBufferType::ELEMENT_ARRAY,
		GLVertexBuffer::EDrawUsage::STATIC
	};

	initializeDefines();
}

uint GLConfig::getTextureBindingPoint(ETextures a_texture)
{
	return textureBindingPoints[uint(a_texture)];
}
GLConstantBuffer::Config GLConfig::getUBOConfig(EUBOs a_ubo)
{
	return uboConfigs[uint(a_ubo)];
}
GLVertexBuffer::Config GLConfig::getVBOConfig(EVBOs a_vbo)
{
	return vboConfigs[uint(a_vbo)];
}
const eastl::vector<eastl::string>& GLConfig::getGlobalShaderDefines()
{
	return defines;
}
uint GLConfig::getMaxLights()
{
	return maxLights;
}
uint GLConfig::getMaxMaterials()
{
	return maxMaterials;
}
GLFramebuffer::EMultiSampleType GLConfig::getMultisampleType()
{
	return multisampleType;
}
void GLConfig::setMultisampleType(GLFramebuffer::EMultiSampleType a_multisampleType)
{
	multisampleType = a_multisampleType;
	GLEngine::graphics->setMultisample(a_multisampleType != GLFramebuffer::EMultiSampleType::NONE);
	initialize();
}

#define TEX_BINDING_POINT_STR(X) StringUtils::to_string(textureBindingPoints[uint(X)])
#define UBO_BINDING_POINT_STR(X) StringUtils::to_string(uboConfigs[uint(X)].bindingPoint)

void GLConfig::initializeDefines()
{
	defines.clear();

	defines.push_back("MAX_MATERIALS "    + StringUtils::to_string(GLConfig::maxMaterials));
	defines.push_back("MAX_LIGHTS "       + StringUtils::to_string(GLConfig::maxLights));
	defines.push_back("NUM_MULTISAMPLES " + StringUtils::to_string(uint(GLConfig::multisampleType)));

	defines.push_back("TEXTURE_ATLAS_ARRAY_BINDING_POINT "  + TEX_BINDING_POINT_STR(ETextures::TextureAtlasArray));
	defines.push_back("DFV_TEXTURE_BINDING_POINT "          + TEX_BINDING_POINT_STR(ETextures::DFVTexture));
	defines.push_back("LIGHT_GRID_TEXTURE_BINDING_POINT "   + TEX_BINDING_POINT_STR(ETextures::ClusteredLightGrid));
	defines.push_back("LIGHT_INDICE_TEXTURE_BINDING_POINT " + TEX_BINDING_POINT_STR(ETextures::ClusteredLightIndice));
	defines.push_back("SHADOW_TEXTURE_BINDING_POINT "       + TEX_BINDING_POINT_STR(ETextures::SunShadow));
	defines.push_back("DEPTH_TEXTURE_BINDING_POINT "        + TEX_BINDING_POINT_STR(ETextures::Depth));
	defines.push_back("HBAO_NOISE_TEXTURE_BINDING_POINT "   + TEX_BINDING_POINT_STR(ETextures::HBAONoise));
	defines.push_back("BLUR_TEXTURE_BINDING_POINT "         + TEX_BINDING_POINT_STR(ETextures::Blur));
	defines.push_back("COLOR_TEXTURE_BINDING_POINT "        + TEX_BINDING_POINT_STR(ETextures::Color));
	defines.push_back("AO_RESULT_TEXTURE_BINDING_POINT "    + TEX_BINDING_POINT_STR(ETextures::HBAOResult));
	defines.push_back("BLOOM_RESULT_TEXTURE_BINDING_POINT " + TEX_BINDING_POINT_STR(ETextures::BloomResult));

	defines.push_back("MODEL_MATRIX_BINDING_POINT "	             + UBO_BINDING_POINT_STR(EUBOs::ModelMatrix));
	defines.push_back("CAMERA_VARS_BINDING_POINT "               + UBO_BINDING_POINT_STR(EUBOs::CameraVars));
	defines.push_back("LIGHTING_GLOBALS_BINDING_POINT "          + UBO_BINDING_POINT_STR(EUBOs::LightingGlobals));
	defines.push_back("MATERIAL_PROPERTIES_BINDING_POINT "       + UBO_BINDING_POINT_STR(EUBOs::MaterialProperties));
	defines.push_back("LIGHT_POSITION_RANGES_BINDING_POINT "     + UBO_BINDING_POINT_STR(EUBOs::ClusteredLightPositionRange));
	defines.push_back("LIGHT_COLOR_INTENSITIES_BINDING_POINT "   + UBO_BINDING_POINT_STR(EUBOs::ClusteredLightColorIntensities));
	defines.push_back("CLUSTERED_SHADING_GLOBALS_BINDING_POINT " + UBO_BINDING_POINT_STR(EUBOs::ClusteredGlobals));
	defines.push_back("HBAO_GLOBALS_BINDING_POINT "              + UBO_BINDING_POINT_STR(EUBOs::HBAOGlobals));
	defines.push_back("SETTINGS_GLOBALS_BINDING_POINT "          + UBO_BINDING_POINT_STR(EUBOs::SettingsGlobals));
}

#undef TEX_BINDING_POINT_STR
#undef UBO_BINDING_POINT_STR

