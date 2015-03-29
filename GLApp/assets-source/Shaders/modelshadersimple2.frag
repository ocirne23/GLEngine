#version 330 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;


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

layout (location = 0) out vec4 out_color;

void main()
{
	MaterialProperty material = u_materialProperties[v_materialID];
	vec3 diffuse = sampleHandle(material.diffuseHandle, v_texcoord).rgb;

	out_color = vec4(diffuse, 1.0);
}
