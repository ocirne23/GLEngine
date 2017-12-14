layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 v_texcoord;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(in_position, 0.0f, 1.0f);
	v_texcoord = in_texcoord;
}
