#version 330 core

layout(location = 0) in vec3 in_position;

out vec3 v_position;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(in_position, 1.0);
	v_position = in_position;
}