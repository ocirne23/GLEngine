#include "Shaders/globals.glsl"
#include "Shaders/clusteredshading.glsl"
#include "Shaders/material.glsl"

in vec3 v_rawPos;
in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
flat in uint v_materialID;
in vec4 v_shadowCoord;

layout (location = 0) out vec3 out_color;

layout (binding = DFV_TEXTURE_BINDING_POINT) uniform sampler2D u_dfvTexture;
layout (binding = 10) uniform sampler2DShadow u_shadowTex;

vec3 rotateNormal(vec3 normal)
{
	normal = normal * 2.0 - 1.0; // Convert from [0 -> 1] to [-1 -> 1]
	vec3 dp1 = dFdx(v_position);
	vec3 dp2 = dFdy(v_position);
	vec2 duv1 = dFdx(v_texcoord);
	vec2 duv2 = dFdy(v_texcoord);
	vec3 T = normalize(dp1 * duv2.y - dp2 * duv1.y);
	vec3 B = normalize(-dp1 * duv2.x + dp2 * duv1.x);
	mat3 rotMat = mat3(T, B, v_normal);
	return normalize(rotMat * normal);
}

// [Burley 2012, "Physically-Based Shading at Disney"]
vec3 diffuseBurley(vec3 diffuse, float Roughness, float NoV, float NoL, float VoH)
{
	float FD90 = 0.5 + 2 * VoH * VoH * Roughness;
	float FdV = 1 + (FD90 - 1) * exp2( (-5.55473 * NoV - 6.98316) * NoV );
	float FdL = 1 + (FD90 - 1) * exp2( (-5.55473 * NoL - 6.98316) * NoL );
	return diffuse / PI * FdV * FdL;
}

// [http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/]
float specularGGX(float roughness, float F0, float NoH, float HoL, float NoL)
{
	float D = texture(u_dfvTexture, vec2(NoH * NoH * NoH * NoH, roughness)).x;
	vec2 FVhelper = texture(u_dfvTexture, vec2(HoL, roughness)).yz;
	float FV = F0 * FVhelper.x + FVhelper.y;
	return D * FV * NoL;
}

float inverseSquareFalloff(float lightDistance, float lightRange)
{
	return pow(max(1.0 - pow((lightDistance / lightRange), 4), 0), 2) / (pow(lightDistance, 2) + 1);
}

vec3 doLight(vec3 lightContrib, vec3 L, vec3 N, vec3 V, float NdotV, float F0, vec3 diffuse, float smoothness, float metalness)
{
	vec3 H = normalize(L + V);
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float HdotL = clamp(dot(H, L), 0.0, 1.0);
	float VdotH = clamp(dot(V, H), 0.0, 1.0);

	vec3 diffuseContrib = diffuseBurley(diffuse, smoothness, NdotV, NdotL, VdotH);
	float specularContrib = specularGGX(smoothness, F0, NdotH, HdotL, NdotL);

	return lightContrib * diffuseContrib + lightContrib * specularContrib;
}

const vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
);

#define PCF_NUM_SAMPLES 16

float PCSS()
{
	float totalShadow = 0.0;
	float bias = 0.0005;
	//float bias = 0.00015;
	for (int i = 0; i < PCF_NUM_SAMPLES; ++i)
	{
		vec2 offset = poissonDisk[i] * 0.0001;
		vec2 texcoord = (v_shadowCoord.xy + offset) / v_shadowCoord.w;
		float depth = (v_shadowCoord.z - bias) / v_shadowCoord.w;
		totalShadow += texture(u_shadowTex, vec3(texcoord, depth));
	}
	return totalShadow / PCF_NUM_SAMPLES;
}

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	if (material.color.a < 0.4 && material.color.a > 0.1) // Hacky transparency for IFC demo
		discard;

	vec3 diffuse = getDiffuseSample(material, v_texcoord).rgb;
	vec3 normal = getNormalSample(material, v_texcoord).rgb;
	vec3 N = rotateNormal(normal);
	vec3 V = normalize(u_eyePos - v_position);
	float NdotV = clamp(dot(N, V), 0.0, 1.0);

	diffuse += material.color.rgb;

	//float smoothness = u_smoothness; //material.roughness;
	//float metalness  = u_metalness;  //material.metalness;
	float smoothness = material.roughness;
	float metalness = material.metalness;

	float albedo = (diffuse.r + diffuse.g + diffuse.b) / 3.0;
	float F0 = mix(0.035, albedo, metalness);

	vec3 lightAccum = vec3(0.0);
	FOR_LIGHT_ITERATOR(light, v_position.z)
	{
		vec3 L = light.positionRange.xyz - v_position;
		float lightDistance = length(L);
		L /= lightDistance;
		float attenuation = inverseSquareFalloff(lightDistance, light.positionRange.w);
		vec3 lightContrib = light.colorIntensity.rgb * light.colorIntensity.a * attenuation;
		lightAccum += doLight(lightContrib, L, N, V, NdotV, F0, diffuse, smoothness, metalness);
	}
	float visibility = PCSS();
	if (visibility > 0.0)
	{
		vec3 sunContrib = u_sunColorIntensity.rgb * u_sunColorIntensity.a * visibility;
		lightAccum += doLight(sunContrib, u_sunDir, N, V, NdotV, F0, diffuse, smoothness, metalness);
	}
	lightAccum += diffuse * u_ambient;
	out_color = vec3(lightAccum);
}

// out_color = texture(u_textureAtlasArray, vec3(gl_FragCoord.xy / vec2(1200, 720), 0)).rgb; // Visualize atlas
