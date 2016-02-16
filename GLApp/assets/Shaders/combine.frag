#include "Shaders/globals.glsl"

layout(binding = COLOR_TEXTURE_BINDING_POINT) uniform FBOSampler u_colorTex;
layout(binding = AO_RESULT_TEXTURE_BINDING_POINT) uniform FBOSampler u_aoTex;
layout(binding = BLOOM_RESULT_TEXTURE_BINDING_POINT) uniform FBOSampler u_bloomTex;

in vec2 v_texcoord;

layout(location = 0) out vec3 out_color;

void main()
{
	vec3 color = sampleFBO(u_colorTex, v_texcoord).rgb;
	float ao = singleSampleFBO(u_aoTex, v_texcoord).r;
	vec3 bloom = singleSampleFBO(u_bloomTex, v_texcoord).rgb;
#if 0
	out_color = vec3(bloom) + vec3(color * ao);
#else

	float exposure = 2.2;
	float brightMax = 1.2;
	
	if (u_hbaoEnabled)
		color *= ao;
	if (u_bloomEnabled)
		color += bloom;
	
	float Y = dot(vec3(0.30, 0.59, 0.11), color);
	float YD = exposure * (exposure / brightMax + 1.0) / (exposure + 1.0);
	color *= YD;
	
	out_color = color;
#endif
}