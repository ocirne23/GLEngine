layout(location = 0) out vec4 out_color;

in vec2 v_texcoord;

uniform sampler2D u_texture;

void main()
{
	out_color = texture(u_texture, v_texcoord);
}
