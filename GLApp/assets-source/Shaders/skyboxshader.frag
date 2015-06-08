#include "Shaders/material.glsl"

////////////////////////// IN / OUT //////////////////////////

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec4 out_color;

////////////////////////// MAIN //////////////////////////

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	vec3 diffuse = getDiffuseSample(material, v_texcoord).rgb;
	out_color = vec4(diffuse, 1.0);
}