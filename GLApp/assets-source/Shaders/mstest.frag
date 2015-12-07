in vec2 v_texcoord;

uniform sampler2DMS u_tex;

layout(location = 0) out vec3 out_color;

void main()
{
	vec2 pos = gl_FragCoord.xy;
	ivec2 ipos = ivec2(pos.x, pos.y);
	//vec3 col = texelFetch(u_tex, ipos, 0).rgb;
	vec3 col = texelFetch(u_tex, ivec2(gl_FragCoord.xy), gl_SampleID).rgb;

	out_color = col;
}