#include "Shaders/globals.glsl"
#include "Shaders/clusteredshading.glsl"
#include "Shaders/material.glsl"

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

layout (binding = DFV_TEXTURE_BINDING_POINT) uniform sampler2D u_dfvTexture;

vec3 rotateNormal(vec3 tangentSpaceNormal)
{
	vec3 normal = tangentSpaceNormal * 2.0 - 1;
	mat3 rotMat = mat3(v_tangent, v_bitangent, v_normal);
	vec3 n = rotMat * vec3(normal);
	return normalize(n);
}

// [Burley 2012, "Physically-Based Shading at Disney"]
vec3 diffuseBurley(vec3 diffuse, float Roughness, float NoV, float NoL, float VoH)
{
	float FD90 = 0.5 + 2 * VoH * VoH * Roughness;
	float FdV = 1 + (FD90 - 1) * exp2( (-5.55473 * NoV - 6.98316) * NoV );
	float FdL = 1 + (FD90 - 1) * exp2( (-5.55473 * NoL - 6.98316) * NoL );
	return diffuse / PI * FdV * FdL; // Divide diffuse color with return value
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
	if (lightRange < 0.0)
		return 1.0;
	else
		return pow(max(1.0 - pow((lightDistance / lightRange), 4), 0), 2) / (pow(lightDistance, 2) + 1);
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

	float roughness = material.roughness;
	float metalness = material.metalness;

	float albedo = (diffuse.r + diffuse.g + diffuse.b) / 3.0;
	float F0 = mix(0.035, albedo, metalness);

	vec3 lightAccum = vec3(0.0);
	FOR_LIGHT_ITERATOR(light, v_position.z)
	{
		vec3 L = light.positionRange.xyz - v_position;
		float lightDistance = length(L);
		L /= lightDistance;
		vec3 H = normalize(L + V);

		float NdotL = clamp(dot(N, L), 0.0, 1.0);
		float NdotH = clamp(dot(N, H), 0.0, 1.0);
		float HdotL = clamp(dot(H, L), 0.0, 1.0);
		float VdotH = clamp(dot(V, H), 0.0, 1.0);

		float attenuation = inverseSquareFalloff(lightDistance, light.positionRange.w);
		vec3 lightContrib = light.colorIntensity.rgb * light.colorIntensity.a * attenuation;
		vec3 diffuseContrib = diffuseBurley(diffuse, roughness, NdotV, NdotL, VdotH);
		float specularContrib = specularGGX(roughness, F0, NdotH, HdotL, NdotL);

		lightAccum += lightContrib * diffuseContrib;
		lightAccum += lightContrib * specularContrib;
	}
	lightAccum += diffuse * u_ambient;
	out_color = lightAccum;
	//out_color = texture(u_textureAtlasArray, vec3(gl_FragCoord.xy / vec2(1200, 720), 0)).rgb; // Visualize atlas
}