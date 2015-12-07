#include "Shaders/globals.glsl"
// GLSL port of Nvidia's HBAO implementation from the DX11 samples
// https://developer.nvidia.com/dx11-samples

#define KERNEL_RADIUS 8.0

in vec2 v_texcoord;

uniform sampler2D u_aoDepthTex;

layout(location = 0) out vec2 out_aoDepth;

vec2 sampleAODepth(vec2 a_uv)
{
	return texture(u_aoDepthTex, v_texcoord + a_uv * u_invFullResolution).rg;
}

float crossBilateralWeight(float a_r, float a_z, float a_z0)
{
    float blurSigma = (KERNEL_RADIUS + 1.0f) * 0.5f;
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

    for(; i <= KERNEL_RADIUS / 2; i += 1.0)
    {
		aoDepth = sampleAODepth(vec2(i, 0));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;

		aoDepth = sampleAODepth(vec2(-i, 0));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;
    }

    for(; i <= KERNEL_RADIUS; i += 2.0)
    {
		aoDepth = sampleAODepth(vec2(0.5 + i, 0));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;

		aoDepth = sampleAODepth(vec2(-0.5 - i, 0));
		w = crossBilateralWeight(i, aoDepth.y, centerZ);
		totalAO += aoDepth.x * w;
		totalWeight += w;
    }
	
    float ao = totalAO / totalWeight;
    out_aoDepth = vec2(ao, centerZ);
}