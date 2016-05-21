#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec2 in_texcoord;
layout(location = 0) out vec4 out_color;

void main() 
{
  out_color = vec4(in_texcoord.xxy, 1.0);
}