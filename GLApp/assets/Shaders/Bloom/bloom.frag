#include "Shaders/globals.glsl"

layout(binding = COLOR_TEXTURE_BINDING_POINT) uniform sampler2D u_colorTex;

in vec2 v_texcoord;

layout(location = 0) out vec3 out_color;

void main()
{
	vec3 color = texture(u_colorTex, v_texcoord).rgb;
    // Check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	out_color = color * brightness;
}