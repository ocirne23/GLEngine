#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in uint in_materialID;

layout(binding = 0) uniform UBO
{
	mat4 u_mvpMatrix;
};

layout (location = 0) out vec2 out_texcoord;

out gl_PerVertex 
{
    vec4 gl_Position;   
};


void main() 
{
	out_texcoord = in_texcoord;
	gl_Position = u_mvpMatrix * vec4(in_position.xyz, 1.0);
}