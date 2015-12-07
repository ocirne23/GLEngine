#include "Shaders/globals.glsl"
// GLSL port of Nvidia's HBAO implementation from the DX11 samples
// https://developer.nvidia.com/dx11-samples

layout(binding = HBAO_DEPTH_TEXTURE_BINDING_POINT) uniform sampler2D u_aoDepthTex;

#if HBAO_USE_MS_FBO
layout(binding = HBAO_COLOR_TEXTURE_BINDING_POINT) uniform sampler2DMS u_colorTex;
#else
layout(binding = HBAO_COLOR_TEXTURE_BINDING_POINT) uniform sampler2D u_colorTex;
#endif

in vec2 v_texcoord;

layout(location = 0) out vec3 out_color;

vec2 sampleAODepth(vec2 a_uv)
{
    return texture(u_aoDepthTex, v_texcoord + a_uv * u_invFullResolution).rg;
}

float crossBilateralWeight(float a_r, float a_z, float a_z0)
{
    float blurSigma = (HBAO_BLUR_KERNEL_RADIUS + 1.0f) * 0.5f;
    float blurFalloff = 1.0f / (2.0f * blurSigma * blurSigma);

    float dz = a_z0 - a_z;
    return exp2(-a_r * a_r * blurFalloff - dz * dz);
}

void main()
{
    vec2 aoDepth = sampleAODepth(vec2(0));
    float centerZ = aoDepth.y;

    float w = 1.0;
    float totalAO = aoDepth.x * w;
    float totalWeight = w;
    float i = 1.0;

    for(; i <= HBAO_BLUR_KERNEL_RADIUS / 2; i += 1.0)
    {
		aoDepth = sampleAODepth(vec2(0, i));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;

		aoDepth = sampleAODepth(vec2(0, -i));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;
    }

    for(; i <= HBAO_BLUR_KERNEL_RADIUS; i += 2.0)
    {
		aoDepth = sampleAODepth(vec2(0, 0.5 + i));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;

		aoDepth = sampleAODepth(vec2(0, -0.5 - i));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;
    }
    float ao = totalAO / totalWeight;

#if HBAO_USE_MS_FBO
	vec3 color = texelFetch(u_colorTex, ivec2(gl_FragCoord.xy), gl_SampleID).rgb;
#else
	vec3 color = texture(u_colorTex, v_texcoord).rgb;
#endif

    out_color = vec3(color * ao); // Combine ao with final scene color
}