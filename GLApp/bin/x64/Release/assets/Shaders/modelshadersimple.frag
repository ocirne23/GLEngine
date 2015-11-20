#include "Shaders/material.glsl"

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

uniform vec3 u_eyePos;
uniform vec3 u_ambient;

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	vec3 diffuse = getDiffuseSample(material, v_texcoord).rgb;
	out_color = diffuse;
	
	//out_color = vec4(vec3(normal), 1.0) + out_color * 0.00000000001; // for testing values without unused variable errors
}