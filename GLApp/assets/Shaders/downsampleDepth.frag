#include "Shaders/globals.glsl"

layout (binding = DEPTH_TEXTURE_BINDING_POINT) uniform FBOSampler u_tex;

in vec2 v_texcoord;

layout (location = 0) out float out_depth;
out float gl_FragDepth;

void main()
{
    out_depth = singleSampleFBO(u_tex, v_texcoord).r;
	gl_FragDepth = out_depth;
}