#include "Shaders/globals.glsl"
#include "Shaders/material.glsl"

////////////////////////// IN / OUT //////////////////////////

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

////////////////////////// MAIN //////////////////////////

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	vec2 texcoord = v_texcoord;
	texcoord.y = texcoord.y * 2.0 - 1.0;
	vec3 diffuse = getDiffuseSample(material, texcoord).rgb * 0.6;
	out_color = diffuse;
}