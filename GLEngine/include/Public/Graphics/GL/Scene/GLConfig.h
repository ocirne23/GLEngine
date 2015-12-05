#pragma once

#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

class GLConfig
{
public:
	enum { MAX_MATERIALS = 200, MAX_LIGHTS = 100 };

	static const GLConstantBuffer::Config MODEL_MATRIX_UBO;
	static const GLConstantBuffer::Config CAMERA_VARS_UBO;
	static const GLConstantBuffer::Config LIGHTING_GLOBALS_UBO;
	static const GLConstantBuffer::Config MATERIAL_PROPERTIES_UBO;
	static const GLConstantBuffer::Config LIGHT_POSITION_RANGES_UBO;
	static const GLConstantBuffer::Config LIGHT_COLOR_INTENSITIES_UBO;
	static const GLConstantBuffer::Config CLUSTERED_SHADING_GLOBALS_UBO;
	static const GLConstantBuffer::Config HBAO_GLOBALS_UBO;

	static const GLVertexBuffer::Config GLMESH_VERTEX_BUFFER;
	static const GLVertexBuffer::Config GLMESH_INDICE_BUFFER;
};
