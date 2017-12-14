layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

out vec3 v_color;

uniform mat4 u_vpMatrix;

void main()
{
	gl_Position = u_vpMatrix * vec4(in_position, 0.0, 1.0);
	v_color	= in_color;
}