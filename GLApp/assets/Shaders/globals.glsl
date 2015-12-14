#ifndef GLOBALS_H
#define GLOBALS_H

/* REQUIRED DEFINES /* // Defined in GLConfig.cpp
PI

MAX_MATERIALS
MAX_LIGHTS
USE_MS_FBO
HBAO_BLUR_KERNEL_RADIUS
HBAO_NUM_DIRECTIONS
HBAO_NUM_STEPS
HBAO_NOISE_TEXTURE_RES

MODEL_MATRIX_BINDING_POINT
CAMERA_VARS_BINDING_POINT
LIGHTING_GLOBALS_BINDING_POINT
MATERIAL_PROPERTIES_BINDING_POINT
LIGHT_POSITION_RANGES_BINDING_POINT
LIGHT_COLOR_INTENSITIES_BINDING_POINT
CLUSTERED_SHADING_GLOBALS_BINDING_POINT
HBAO_GLOBALS_BINDING_POINT

TEXTURE_ATLAS_ARRAY_BINDING_POINT
DFV_TEXTURE_BINDING_POINT
LIGHT_GRID_TEXTURE_BINDING_POINT
LIGHT_INDICE_TEXTURE_BINDING_POINT
HBAO_DEPTH_TEXTURE_BINDING_POINT
HBAO_COLOR_TEXTURE_BINDING_POINT
HBAO_BLUR_TEXTURE_BINDING_POINT
*/

struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 normalTexMapping;
	int diffuseAtlasNr;
	int normalAtlasNr;
	float roughness;
	float metalness;
	vec4 color;
};

/* ALL UBOS */
layout (std140, binding = MODEL_MATRIX_BINDING_POINT) uniform ModelMatrix
{
	mat4 u_modelMatrix;
};
layout (std140, binding = CAMERA_VARS_BINDING_POINT) uniform CameraVars
{
	mat4 u_vpMatrix;
	mat4 u_viewMatrix;
	mat4 u_normalMatrix;
	vec3 u_eyePos;
	float padding_CameraVars;
};
layout (std140, binding = LIGHTING_GLOBALS_BINDING_POINT) uniform LightingGlobals
{
	vec3 u_ambient;
	float padding_LightningGlobals;
	vec3 u_sunDir;
	float padding2_LightingGlobals;
	vec4 u_sunColorIntensity;
	mat4 u_shadowMat;
};
layout (std140, binding = MATERIAL_PROPERTIES_BINDING_POINT) uniform MaterialProperties
{
	MaterialProperty u_materialProperties[MAX_MATERIALS];
};

layout (std140, binding = LIGHT_POSITION_RANGES_BINDING_POINT) uniform LightPositionRanges
{
	vec4 u_lightPositionRanges[MAX_LIGHTS];
};
layout (std140, binding = LIGHT_COLOR_INTENSITIES_BINDING_POINT) uniform LightColorsIntensities
{
	vec4 u_lightColorIntensities[MAX_LIGHTS];
};
layout (std140, binding = CLUSTERED_SHADING_GLOBALS_BINDING_POINT) uniform ClusteredShadingGlobals
{
	float u_recNear;
	float u_recLogSD1;

	int u_tileWidth;
	int u_tileHeight;

	int u_gridHeight;
	int u_gridDepth;
};
layout (std140, binding = HBAO_GLOBALS_BINDING_POINT) uniform HBAOGlobals
{
	vec2 u_fullResolution;
	vec2 u_invFullResolution;

	vec2 u_aoResolution;
	vec2 u_invAOResolution;

	vec2 u_focalLen;
	vec2 u_invFocalLen;

	vec2 u_uvToViewA;
	vec2 u_uvToViewB;

	float u_r;
	float u_r2;
	float u_negInvR2;
	float u_maxRadiusPixels;

	float u_angleBias;
	float u_tanAngleBias;
	float u_strength;
	float padding_HBAOGlobals;

	vec2 u_noiseTexScale;
	vec2 u_ndcDepthConv;
};

#endif // GLOBALS_H