uniform sampler2D u_tex;
uniform sampler2DArray u_textureArray;

in vec2 v_texcoord;

layout (location = 0) out vec4 out_color;

void main()
{
	out_color = texture(u_textureArray, vec3(v_texcoord, 0));
	//out_color = texture(u_tex, v_texcoord);
	//out_color = vec4(0.0, 1.0, 0.0, 1.0);
	//out_color = vec4(v_texcoord.xxy, 1.0);
}