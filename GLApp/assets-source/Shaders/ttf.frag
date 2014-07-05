#version 330

layout(location = 0) out vec4 out_color;

in vec2 v_texcoord;
in vec4 v_color;

uniform sampler2D u_texture;

void main()
{
	float value = texture(u_texture, v_texcoord).r;
	out_color = vec4(v_color.rgb * value, value);
}
