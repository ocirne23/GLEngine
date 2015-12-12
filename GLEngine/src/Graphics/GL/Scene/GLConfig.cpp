#include "Graphics/GL/Scene/GLConfig.h"

#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/Utils/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"
#include "EASTL/to_string.h"

eastl::vector<eastl::string> GLConfig::defines;

const uint GLConfig::CLUSTERED_SHADING_TILE_WIDTH  = 64;
const uint GLConfig::CLUSTERED_SHADING_TILE_HEIGHT = 64;
const uint GLConfig::HBAO_NOISE_TEXTURE_RES        = 8;
const GLFramebuffer::EMultiSampleType GLConfig::MULTISAMPLE_TYPE = GLFramebuffer::EMultiSampleType::NONE;

// Global shader defines
const uint GLConfig::MAX_MATERIALS = 200;
const uint GLConfig::MAX_LIGHTS    = 100;
const uint GLConfig::USE_MS_FBO    = (GLConfig::MULTISAMPLE_TYPE == GLFramebuffer::EMultiSampleType::NONE) ? 0 : 1;

const float GLConfig::HBAO_BLUR_KERNEL_RADIUS = 8.0f;
const uint GLConfig::HBAO_NUM_DIRECTIONS      = 6;
const uint GLConfig::HBAO_NUM_STEPS           = 6;

const uint GLConfig::TEXTURE_ATLAS_ARRAY_BINDING_POINT  = 1;
const uint GLConfig::DFV_TEXTURE_BINDING_POINT          = 2;
const uint GLConfig::LIGHT_GRID_TEXTURE_BINDING_POINT   = 3;
const uint GLConfig::LIGHT_INDICE_TEXTURE_BINDING_POINT = 4;
const uint GLConfig::DEPTH_TEXTURE_BINDING_POINT        = 5;
const uint GLConfig::HBAO_NOISE_TEXTURE_BINDING_POINT   = 6;
const uint GLConfig::BLUR_TEXTURE_BINDING_POINT         = 7;

const uint GLConfig::COLOR_TEXTURE_BINDING_POINT        = 1;
const uint GLConfig::AO_RESULT_TEXTURE_BINDING_POINT    = 2;
const uint GLConfig::BLOOM_RESULT_TEXTURE_BINDING_POINT = 3;

const GLConstantBuffer::Config GLConfig::MODEL_MATRIX_UBO =              {0, "ModelMatrix",             GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::ModelMatrixUBO)};
const GLConstantBuffer::Config GLConfig::CAMERA_VARS_UBO =               {1, "CameraVars",              GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::CameraVarsUBO)};
const GLConstantBuffer::Config GLConfig::LIGHTING_GLOBALS_UBO =          {2, "LightingGlobals",         GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLRenderer::LightingGlobalsUBO)};
const GLConstantBuffer::Config GLConfig::MATERIAL_PROPERTIES_UBO =       {3, "MaterialProperties",      GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLMaterial) * MAX_MATERIALS};
const GLConstantBuffer::Config GLConfig::LIGHT_POSITION_RANGES_UBO =     {4, "LightPositionRanges",     GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * MAX_LIGHTS};
const GLConstantBuffer::Config GLConfig::LIGHT_COLOR_INTENSITIES_UBO =   {5, "LightColorIntensities",   GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * MAX_LIGHTS};
const GLConstantBuffer::Config GLConfig::CLUSTERED_SHADING_GLOBALS_UBO = {6, "ClusteredShadingGlobals", GLConstantBuffer::EDrawUsage::STATIC, sizeof(ClusteredShading::GlobalsUBO)};
const GLConstantBuffer::Config GLConfig::HBAO_GLOBALS_UBO =              {7, "HBAOGlobals",             GLConstantBuffer::EDrawUsage::STATIC, sizeof(HBAO::GlobalsUBO)};
// !Global shader defines

BEGIN_UNNAMED_NAMESPACE()
VertexAttribute GLMESH_VB_ATTRIBS[] = {
	VertexAttribute(0, VertexAttribute::EFormat::FLOAT, 3),       // Position
	VertexAttribute(1, VertexAttribute::EFormat::FLOAT, 2),       // Texcoord
	VertexAttribute(2, VertexAttribute::EFormat::FLOAT, 3),       // Normals
	VertexAttribute(3, VertexAttribute::EFormat::FLOAT, 3),       // Tangents
	VertexAttribute(4, VertexAttribute::EFormat::FLOAT, 3),       // Bitangents
	VertexAttribute(5, VertexAttribute::EFormat::UNSIGNED_INT, 1) // MaterialID
};
END_UNNAMED_NAMESPACE()

const GLVertexBuffer::Config GLConfig::GLMESH_VERTEX_BUFFER = {
	GLVertexBuffer::EBufferType::ARRAY, 
	GLVertexBuffer::EDrawUsage::STATIC, 
	eastl::vector<VertexAttribute>(GLMESH_VB_ATTRIBS, GLMESH_VB_ATTRIBS + ARRAY_SIZE(GLMESH_VB_ATTRIBS))
};

const GLVertexBuffer::Config GLConfig::GLMESH_INDICE_BUFFER = {
	GLVertexBuffer::EBufferType::ELEMENT_ARRAY, 
	GLVertexBuffer::EDrawUsage::STATIC
};

const eastl::vector<eastl::string>& GLConfig::getGlobalShaderDefines()
{
	if (!defines.size())
	{
		defines.push_back("PI 3.14159265");

		defines.push_back("MAX_MATERIALS " + StringUtils::to_string(GLConfig::MAX_MATERIALS));
		defines.push_back("MAX_LIGHTS "    + StringUtils::to_string(GLConfig::MAX_LIGHTS));
		defines.push_back("USE_MS_FBO "    + StringUtils::to_string(GLConfig::USE_MS_FBO));

		defines.push_back("HBAO_BLUR_KERNEL_RADIUS " + StringUtils::to_string(GLConfig::HBAO_BLUR_KERNEL_RADIUS));
		defines.push_back("HBAO_NUM_DIRECTIONS "     + StringUtils::to_string(GLConfig::HBAO_NUM_DIRECTIONS));
		defines.push_back("HBAO_NUM_STEPS "          + StringUtils::to_string(GLConfig::HBAO_NUM_STEPS));
		defines.push_back("HBAO_NOISE_TEXTURE_RES "  + StringUtils::to_string(GLConfig::HBAO_NOISE_TEXTURE_RES));

		defines.push_back("TEXTURE_ATLAS_ARRAY_BINDING_POINT "  + StringUtils::to_string(GLConfig::TEXTURE_ATLAS_ARRAY_BINDING_POINT));
		defines.push_back("DFV_TEXTURE_BINDING_POINT "          + StringUtils::to_string(GLConfig::DFV_TEXTURE_BINDING_POINT));
		defines.push_back("LIGHT_GRID_TEXTURE_BINDING_POINT "   + StringUtils::to_string(GLConfig::LIGHT_GRID_TEXTURE_BINDING_POINT));
		defines.push_back("LIGHT_INDICE_TEXTURE_BINDING_POINT " + StringUtils::to_string(GLConfig::LIGHT_INDICE_TEXTURE_BINDING_POINT));
		defines.push_back("DEPTH_TEXTURE_BINDING_POINT "        + StringUtils::to_string(GLConfig::DEPTH_TEXTURE_BINDING_POINT));
		defines.push_back("HBAO_NOISE_TEXTURE_BINDING_POINT "   + StringUtils::to_string(GLConfig::HBAO_NOISE_TEXTURE_BINDING_POINT));
		defines.push_back("BLUR_TEXTURE_BINDING_POINT "         + StringUtils::to_string(GLConfig::BLUR_TEXTURE_BINDING_POINT));
		
		defines.push_back("COLOR_TEXTURE_BINDING_POINT "        + StringUtils::to_string(GLConfig::COLOR_TEXTURE_BINDING_POINT));
		defines.push_back("AO_RESULT_TEXTURE_BINDING_POINT "    + StringUtils::to_string(GLConfig::AO_RESULT_TEXTURE_BINDING_POINT));
		defines.push_back("BLOOM_RESULT_TEXTURE_BINDING_POINT " + StringUtils::to_string(GLConfig::BLOOM_RESULT_TEXTURE_BINDING_POINT));

		defines.push_back("MODEL_MATRIX_BINDING_POINT "              + StringUtils::to_string(GLConfig::MODEL_MATRIX_UBO.bindingPoint));
		defines.push_back("CAMERA_VARS_BINDING_POINT "               + StringUtils::to_string(GLConfig::CAMERA_VARS_UBO.bindingPoint));
		defines.push_back("LIGHTING_GLOBALS_BINDING_POINT "          + StringUtils::to_string(GLConfig::LIGHTING_GLOBALS_UBO.bindingPoint));
		defines.push_back("MATERIAL_PROPERTIES_BINDING_POINT "       + StringUtils::to_string(GLConfig::MATERIAL_PROPERTIES_UBO.bindingPoint));
		defines.push_back("LIGHT_POSITION_RANGES_BINDING_POINT "     + StringUtils::to_string(GLConfig::LIGHT_POSITION_RANGES_UBO.bindingPoint));
		defines.push_back("LIGHT_COLOR_INTENSITIES_BINDING_POINT "   + StringUtils::to_string(GLConfig::LIGHT_COLOR_INTENSITIES_UBO.bindingPoint));
		defines.push_back("CLUSTERED_SHADING_GLOBALS_BINDING_POINT " + StringUtils::to_string(GLConfig::CLUSTERED_SHADING_GLOBALS_UBO.bindingPoint));
		defines.push_back("HBAO_GLOBALS_BINDING_POINT "              + StringUtils::to_string(GLConfig::HBAO_GLOBALS_UBO.bindingPoint));
	}
	return defines;
}