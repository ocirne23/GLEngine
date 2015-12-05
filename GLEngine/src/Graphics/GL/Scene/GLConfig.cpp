#include "Graphics/GL/Scene/GLConfig.h"

#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/Utils/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"

const GLConstantBuffer::Config GLConfig::MODEL_MATRIX_UBO =              {0, "ModelMatrix",             GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::ModelMatrixUBO)};
const GLConstantBuffer::Config GLConfig::CAMERA_VARS_UBO =               {1, "CameraVars",              GLConstantBuffer::EDrawUsage::STREAM, sizeof(GLRenderer::CameraVarsUBO)};
const GLConstantBuffer::Config GLConfig::LIGHTING_GLOBALS_UBO =          {2, "LightingGlobals",         GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLRenderer::LightingGlobalsUBO)};
const GLConstantBuffer::Config GLConfig::MATERIAL_PROPERTIES_UBO =       {3, "MaterialProperties",      GLConstantBuffer::EDrawUsage::STATIC, sizeof(GLMaterial) * MAX_MATERIALS};
const GLConstantBuffer::Config GLConfig::LIGHT_POSITION_RANGES_UBO =     {4, "LightPositionRanges",     GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * MAX_LIGHTS};
const GLConstantBuffer::Config GLConfig::LIGHT_COLOR_INTENSITIES_UBO =   {5, "LightColorIntensities",   GLConstantBuffer::EDrawUsage::STREAM, sizeof(glm::vec4) * MAX_LIGHTS};
const GLConstantBuffer::Config GLConfig::CLUSTERED_SHADING_GLOBALS_UBO = {6, "ClusteredShadingGlobals", GLConstantBuffer::EDrawUsage::STATIC, sizeof(ClusteredShading::GlobalsUBO)};
const GLConstantBuffer::Config GLConfig::HBAO_GLOBALS_UBO =              {7, "HBAOGlobals",             GLConstantBuffer::EDrawUsage::STATIC, sizeof(HBAO::GlobalsUBO)};

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