#include "Shaders/globals.glsl"

layout(binding = COLOR_TEXTURE_BINDING_POINT) uniform FBOSampler u_colorTex;

in vec2 v_texcoord;

layout(location = 0) out vec3 out_color;

void main()
{
	vec3 color = singleSampleFBO(u_colorTex, v_texcoord).rgb;
	float luminance = (color.r + color.g + color.b) / 3.0;
	vec3 L = color * (vec3(0.9) / log(luminance));
	out_color = L / (1 + L);
	//out_color = color;
}   
// float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
