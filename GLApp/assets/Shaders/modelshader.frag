#version 330 core

/* REQUIRED DEFINES /*
* LIGHT_GRID_WIDTH
* LIGHT_GRID_HEIGHT
* LIGHT_GRID_TILE_WIDTH
* LIGHT_GRID_TILE_HEIGHT
* MAX_LIGHTS
*/
////////////////////////// IN / OUT //////////////////////////

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;
#ifdef SSAO_ENABLED
in vec3 v_screenSpaceNormal;
#endif

layout (location = 0) out vec4 out_color;
#ifdef SSAO_ENABLED
layout (location = 1) out vec3 out_normal;
#endif

////////////////////////// MATERIAL LIGHTING //////////////////////////

#define NUM_TEXTURE_ARRAYS 16
#define MaterialHandle uvec2

const uint MAX_MATERIALS = 200u;
const uint INVALID_MATERIAL = 0xFFFFFFFFu;
const float DEFAULT_SPECULAR = 0.2;
const float SPECULAR_POWER = 1.2;
const float ATTENUATION = 0.004;
const float BUMP_MAP_STRENGTH = 1.2;

struct MaterialProperty
{
	vec4 diffuseColorAndAlpha;
	vec4 specularColorAndExp;
	vec4 materialEmissiveAndPower;
	MaterialHandle diffuseHandle;
	MaterialHandle bumpHandle;
	MaterialHandle specularHandle;
	MaterialHandle maskHandle;
};

uniform sampler2DArray u_textureData[NUM_TEXTURE_ARRAYS];

layout (std140) uniform MaterialProperties
{
	MaterialProperty u_materialProperties[MAX_MATERIALS];
};

bool hasTexture(MaterialHandle handle)
{
	return handle.x != INVALID_MATERIAL;
}

vec4 sampleHandle(MaterialHandle handle, vec2 texcoord)
{
#ifdef DYNAMIC_INDEXING
	return texture(u_textureData[handle.x], vec3(texcoord, handle.y));
#else
	uint index = handle.x;
	if (index == 0u) return texture(u_textureData[0], vec3(texcoord, handle.y));
	else if (index == 1u) return texture(u_textureData[1], vec3(texcoord, handle.y));
	else if (index == 2u) return texture(u_textureData[2], vec3(texcoord, handle.y));
	else if (index == 3u) return texture(u_textureData[3], vec3(texcoord, handle.y));
	else if (index == 4u) return texture(u_textureData[4], vec3(texcoord, handle.y));
	else if (index == 5u) return texture(u_textureData[5], vec3(texcoord, handle.y));
	else if (index == 6u) return texture(u_textureData[6], vec3(texcoord, handle.y));
	else if (index == 7u) return texture(u_textureData[7], vec3(texcoord, handle.y));
	else if (index == 8u) return texture(u_textureData[8], vec3(texcoord, handle.y));
	else if (index == 9u) return texture(u_textureData[9], vec3(texcoord, handle.y));
	else if (index == 10u) return texture(u_textureData[10], vec3(texcoord, handle.y));
	else if (index == 11u) return texture(u_textureData[11], vec3(texcoord, handle.y));
	else if (index == 12u) return texture(u_textureData[12], vec3(texcoord, handle.y));
	else if (index == 13u) return texture(u_textureData[13], vec3(texcoord, handle.y));
	else if (index == 14u) return texture(u_textureData[14], vec3(texcoord, handle.y));
	else if (index == 15u) return texture(u_textureData[15], vec3(texcoord, handle.y));
	else return texture(u_textureData[0], vec3(texcoord, handle.y));
#endif
}

ivec2 getTextureSize(MaterialHandle handle)
{
#ifdef DYNAMIC_INDEXING
	return textureSize(u_textureData[handle.x], 0).xy;
#else
	uint index = handle.x;
	if (index == 0u) return textureSize(u_textureData[0], 0).xy;
	else if (index == 1u) return textureSize(u_textureData[1], 0).xy;
	else if (index == 2u) return textureSize(u_textureData[2], 0).xy;
	else if (index == 3u) return textureSize(u_textureData[3], 0).xy;
	else if (index == 4u) return textureSize(u_textureData[4], 0).xy;
	else if (index == 5u) return textureSize(u_textureData[5], 0).xy;
	else if (index == 6u) return textureSize(u_textureData[6], 0).xy;
	else if (index == 7u) return textureSize(u_textureData[7], 0).xy;
	else if (index == 8u) return textureSize(u_textureData[8], 0).xy;
	else if (index == 9u) return textureSize(u_textureData[9], 0).xy;
	else if (index == 10u) return textureSize(u_textureData[10], 0).xy;
	else if (index == 11u) return textureSize(u_textureData[11], 0).xy;
	else if (index == 12u) return textureSize(u_textureData[12], 0).xy;
	else if (index == 13u) return textureSize(u_textureData[13], 0).xy;
	else if (index == 14u) return textureSize(u_textureData[14], 0).xy;
	else if (index == 15u) return textureSize(u_textureData[15], 0).xy;
	else return textureSize(u_textureData[0], 0).xy;
#endif
}

vec3 getBumpedNormal(MaterialProperty material)
{
	const vec3 offsetDir = vec3(-1, 0, 1);
	vec2 offset = 1.0 / getTextureSize(material.bumpHandle);

	float s11 = sampleHandle(material.bumpHandle, v_texcoord).r; //Height at center
	float s01 = sampleHandle(material.bumpHandle, v_texcoord + offset * offsetDir.xy).r; //Left
	float s21 = sampleHandle(material.bumpHandle, v_texcoord + offset * offsetDir.zy).r; //Right
	float s10 = sampleHandle(material.bumpHandle, v_texcoord + offset * offsetDir.yx).r; //Down
	float s12 = sampleHandle(material.bumpHandle, v_texcoord + offset * offsetDir.yz).r; //Up
	vec3 va = normalize(vec3(BUMP_MAP_STRENGTH, 0.0, s21 - s01));
	vec3 vb = normalize(vec3(0.0, BUMP_MAP_STRENGTH, s12 - s10));
	vec4 bump = vec4(cross(va, vb), s11);

	mat3 rotMat = mat3(v_tangent, v_bitangent, v_normal);
	return rotMat * vec3(bump);
}

////////////////////////// CLUSTERED SHADING //////////////////////////

uniform isamplerBuffer u_lightIndices;
uniform isamplerBuffer u_lightGrid;
uniform vec3 u_ambient;
uniform float u_recNear;
uniform float u_recLogSD1;

layout (std140) uniform LightPositionRanges
{
	vec4 u_lightPositionRanges[MAX_LIGHTS];
};
layout (std140) uniform LightColors
{
	vec4 u_lightColors[MAX_LIGHTS];
};

ivec2 getLightListBeginEnd(float viewspaceDepth)
{
	// i and j coordinates are just the same as tiled shading, and based on screen space position.
	ivec2 tileXY = ivec2(int(gl_FragCoord.x) / LIGHT_GRID_TILE_WIDTH, int(gl_FragCoord.y) / LIGHT_GRID_TILE_HEIGHT);

	// k is based on the log of the view space Z coordinate.
	float tileZ = log(-viewspaceDepth * u_recNear) * u_recLogSD1;

	ivec3 clusterPos = ivec3(tileXY, int(tileZ));
	int offset = (clusterPos.x * LIGHT_GRID_HEIGHT + clusterPos.y) * LIGHT_GRID_DEPTH + clusterPos.z;

	return texelFetch(u_lightGrid, offset).xy;
}

////////////////////////// MAIN //////////////////////////

#define PI 3.1415926535

uniform vec3 u_eyePos;

float G1V(float dotNV, float k)
{
	return 1.0 / (dotNV * (1.0 - k ) + k);
}

void main()
{
	MaterialProperty material = u_materialProperties[v_materialID];
	
	if (hasTexture(material.maskHandle) && sampleHandle(material.maskHandle, v_texcoord).r < 0.5)
	{
		discard;
	}
	
	vec3 diffuse = sampleHandle(material.diffuseHandle, v_texcoord).rgb * material.diffuseColorAndAlpha.rgb;
	vec3 normal = hasTexture(material.bumpHandle) ? getBumpedNormal(material) : v_normal;
	float specular = hasTexture(material.specularHandle) ? sampleHandle(material.specularHandle, v_texcoord).r : DEFAULT_SPECULAR;
	
	vec3 diffuseAccum = vec3(0);
	vec3 specularAccum = vec3(0);
	
	vec3 N = normalize(normal);
	vec3 V = normalize(u_eyePos - v_position);
		
	ivec2 lightsBeginEnd = getLightListBeginEnd(v_position.z); 

	int lightsBegin = lightsBeginEnd.x;
	int lightsEnd = lightsBeginEnd.y;

	for (int i = lightsBegin; i < lightsEnd; ++i)
	{
		int lightIndex = texelFetch(u_lightIndices, i).r; 

		vec4 lightPosViewspaceRange = u_lightPositionRanges[lightIndex];
		vec4 lightColor = u_lightColors[lightIndex];

		vec3 lightPosViewspace = lightPosViewspaceRange.xyz;
		float lightRange = lightPosViewspaceRange.w;

		vec3 lightVec = (lightPosViewspace - v_position);
		float dist = length(lightVec);
		
		/* Linear falloff, not physically based */
		float atten = max(1.0 - max(0.0, dist / lightRange), 0.0);
		
		vec3 L = lightVec / dist;
		vec3 H = normalize(L + V);
		
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		float NoH = clamp(dot(N, H), 0.0, 1.0);
		float HoL = clamp(dot(H, L), 0.0, 1.0);

		vec3 diffuseContrib = diffuse * atten * lightColor.rgb * NoL;
		diffuseAccum += diffuseContrib;

		float F, D, vis;
		float roughness = 1.0 - specular;
		float alpha = roughness * roughness;
		float alphaSqr = alpha * alpha;
		
		float denom = NoH * NoH * (alphaSqr - 1.0) + 1.0;
		D = alphaSqr / (PI * denom * denom);

		float F0 = 0.04; // refraction index
		float dotLH5 = exp2(-8.65617024533378044416 * HoL);
		F = F0 + (1.0 - F0) * dotLH5;

		float k = alpha / 2.0;
		float visSqrt = G1V(NoL, k);
		vis = visSqrt * visSqrt;

		specularAccum += diffuseContrib * F * NoL * D * vis;
	}
	
	diffuseAccum += diffuse * u_ambient;
	out_color = vec4(diffuseAccum + specularAccum, 1.0);
	//out_color = vec4(0.4, 0.4, 0.4, 1.0);
#ifdef SSAO_ENABLED
	out_normal = vec3(v_screenSpaceNormal);
#endif
}
