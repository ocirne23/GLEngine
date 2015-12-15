#include "Shaders/globals.glsl"

layout(binding = COLOR_TEXTURE_BINDING_POINT) uniform sampler2D u_colorTex;
layout(binding = AO_RESULT_TEXTURE_BINDING_POINT) uniform sampler2D u_aoTex;
layout(binding = BLOOM_RESULT_TEXTURE_BINDING_POINT) uniform sampler2D u_bloomTex;

in vec2 v_texcoord;

layout(location = 0) out vec3 out_color;

void main()
{
	vec3 color = texture(u_colorTex, v_texcoord).rgb;
	float ao = texture(u_aoTex, v_texcoord).r;
	vec3 bloom = texture(u_bloomTex, v_texcoord).rgb;
#if 0
	out_color = vec3(bloom) + vec3(color * ao);
#else
	float exposure = 2.0;
	float brightMax = 1.0;
	float bloomFactor = 0.5;
	
	color *= ao;
	color += bloom * bloomFactor;
	
	float Y = dot(vec3(0.30, 0.59, 0.11), color);
	float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
	color *= YD;
	
	out_color = color;
#endif
}