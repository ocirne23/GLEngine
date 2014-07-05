#version 330 core

layout(location = 0) in vec3 in_position;

out vec3 position;

void main(void) 
{
	position = in_position;
}