#include "Shaders/globals.glsl"
#include "Shaders/material.glsl"

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	if (material.color.a < 0.4 && material.color.a > 0.1)
		discard;
	
	vec3 diffuse = getDiffuseSample(material, v_texcoord).rgb;
	diffuse += material.color.rgb;
	out_color = vec3(diffuse);
}