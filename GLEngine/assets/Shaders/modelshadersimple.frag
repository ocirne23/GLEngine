#version 330 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

const uint MAX_MATERIALS = 200u;

#define MaterialHandle uvec2

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

layout (std140) uniform MaterialProperties
{
	MaterialProperty u_materialProperties[MAX_MATERIALS];
};

layout (location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(0.0, 1.0, 1.0, 1.0);
}
