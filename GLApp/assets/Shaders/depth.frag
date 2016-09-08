#include "Shaders/globals.glsl"
#include "Shaders/material.glsl"

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
	out_color = vec3(0.0);
}