#ifndef GLOBALS_H
#define GLOBALS_H

#if NUM_MULTISAMPLES > 0
	#if FRAGMENT_SHADER
		vec4 textureMultisampleOnce(sampler2DMS sampler, vec2 pos)
		{
			ivec2 ipos = ivec2(pos * textureSize(sampler).xy);
			return texelFetch(sampler, ipos, 0);
		}
		vec4 textureMultisample(sampler2DMS sampler, vec2 pos)
		{
			ivec2 ipos = ivec2(pos * textureSize(sampler).xy);

			vec4 color = vec4(0.0);
			
			for (int i = 0; i < NUM_MULTISAMPLES; i++)
			{
				color += texelFetch(sampler, ipos, i);
			}
			
			color /= float(NUM_MULTISAMPLES);
			
			return color;
		}
	#endif // FRAGMENT_SHADER
	#define FBOSampler sampler2DMS
	#define sampleFBO textureMultisample
	#define singleSampleFBO textureMultisampleOnce
#else // ! NUM_MULTISAMPLES > 0
	#define FBOSampler sampler2D
	#define sampleFBO texture
	#define singleSampleFBO texture
#endif

struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 normalTexMapping;
	int diffuseAtlasNr;
	int normalAtlasNr;
	float smoothness;
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
	float u_camNear;
	float u_camFar;
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
layout (std140, binding = SETTINGS_GLOBALS_BINDING_POINT) uniform SettingsGlobals
{
	int u_hbaoEnabled;
	int u_bloomEnabled;
	int u_shadowsEnabled;
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