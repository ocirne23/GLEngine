#pragma once

#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "EASTL/string.h"

class GLConfig
{
public:

	static const eastl::vector<eastl::string>& getGlobalShaderDefines();

public:

	static const uint CLUSTERED_SHADING_TILE_WIDTH;
	static const uint CLUSTERED_SHADING_TILE_HEIGHT;
	static const uint HBAO_NOISE_TEXTURE_RES;
	static const GLFramebuffer::EMultiSampleType MULTISAMPLE_TYPE;

public:

	// Part of global shader defines
	static const uint MAX_MATERIALS;
	static const uint MAX_LIGHTS;

	static const float HBAO_BLUR_KERNEL_RADIUS;
	static const uint HBAO_USE_MS_FBO;
	static const uint HBAO_NUM_DIRECTIONS;
	static const uint HBAO_NUM_STEPS;

	static const uint TEXTURE_ATLAS_ARRAY_BINDING_POINT;
	static const uint DFV_TEXTURE_BINDING_POINT;
	static const uint LIGHT_GRID_TEXTURE_BINDING_POINT;
	static const uint LIGHT_INDICE_TEXTURE_BINDING_POINT;
	static const uint HBAO_NOISE_TEXTURE_BINDING_POINT;
	static const uint HBAO_DEPTH_TEXTURE_BINDING_POINT;
	static const uint HBAO_COLOR_TEXTURE_BINDING_POINT;

	static const GLConstantBuffer::Config MODEL_MATRIX_UBO;
	static const GLConstantBuffer::Config CAMERA_VARS_UBO;
	static const GLConstantBuffer::Config LIGHTING_GLOBALS_UBO;
	static const GLConstantBuffer::Config MATERIAL_PROPERTIES_UBO;
	static const GLConstantBuffer::Config LIGHT_POSITION_RANGES_UBO;
	static const GLConstantBuffer::Config LIGHT_COLOR_INTENSITIES_UBO;
	static const GLConstantBuffer::Config CLUSTERED_SHADING_GLOBALS_UBO;
	static const GLConstantBuffer::Config HBAO_GLOBALS_UBO;
	
public:

	static const GLVertexBuffer::Config GLMESH_VERTEX_BUFFER;
	static const GLVertexBuffer::Config GLMESH_INDICE_BUFFER;

private:

	GLConfig() {}
};
