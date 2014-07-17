#version 330 core

layout(location = 0) in vec4 in_position;

uniform mat4 u_mvp;
uniform mat4 u_transform;

void main()
{
	vec4 pos = u_transform * in_position;
	gl_Position = u_mvp * pos;
}