#version 330 core

uniform sampler2D u_colorTex;
uniform sampler2D u_ssaoDepthTex;

uniform int u_blurSize = 4; // use size of noise texture

in vec2 v_texcoord;

layout (location = 0) out vec4 out_color;


void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(u_colorTex, 0));
	float result = 0.0;
	vec2 hlim = vec2(float(-u_blurSize) * 0.5 + 0.5);
	for (int i = 0; i < u_blurSize; ++i) 
	{
		for (int j = 0; j < u_blurSize; ++j) 
		{
			vec2 offset = (hlim + vec2(float(i), float(j))) * texelSize;
			result += texture(u_ssaoDepthTex, v_texcoord + offset).r;
		}
	}
 
	float blurred = result / float(u_blurSize * u_blurSize);

	vec3 color = texture(u_colorTex, v_texcoord).rgb;
	color *= (1.0 - blurred);

	out_color = vec4(vec3(color), 1.0);
}