#version 330 core

uniform sampler2D u_tex;

in vec2 v_texcoord;

layout (location = 0) out vec4 out_color;

void main()
{
//	out_color = texture(u_tex, v_texcoord);
	out_color = vec4(1.0, 0.0, 0.0, 1.0);
}