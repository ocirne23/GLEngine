#ifndef GLOBALS_H
#define GLOBALS_H

/* REQUIRED DEFINES */ // Define in app?
#define MAX_MATERIALS	200
#define MAX_LIGHTS		100

#define MODEL_MATRIX_BINDING_POINT              0
#define CAMERA_VARS_BINDING_POINT               1
#define LIGHTING_GLOBALS_BINDING_POINT          2

#define MATERIAL_PROPERTIES_BINDING_POINT       3
#define LIGHT_POSITION_RANGES_BINDING_POINT     4
#define LIGHT_COLOR_INTENSITIES_BINDING_POINT   5

#define CLUSTERED_SHADING_GLOBALS_BINDING_POINT 6
#define HBAO_GLOBALS_BINDING_POINT              7

/* GENERIC DEFINES */
#define PI 3.14159265

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
};
layout (std140, binding = MATERIAL_PROPERTIES_BINDING_POINT) uniform MaterialProperties
{
	struct MaterialProperty
	{
		vec4 diffuseTexMapping;
		vec4 normalTexMapping;
		int diffuseAtlasNr;
		int normalAtlasNr;
		float roughness;
		float metalness;
		vec4 color;
	} u_materialProperties[MAX_MATERIALS];
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

#endif GLOBALS_H