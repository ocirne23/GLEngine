#include "Shaders/globals.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(location = 5) in uint in_materialID;

out vec3 v_position;
out vec2 v_texcoord;
flat out uint v_materialID;

void main()
{
	mat4 mat = u_vpMatrix * u_modelMatrix;
	mat[3] = vec4(u_eyePos, 1.0); // Move skybox to eye position
	gl_Position = mat * vec4(in_position, 1.0);
	v_texcoord	 = in_texcoord;
	v_materialID = in_materialID;
}