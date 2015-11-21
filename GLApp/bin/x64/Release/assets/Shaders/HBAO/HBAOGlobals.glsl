#ifndef HBAO_GLOBALS
#define HBAO_GLOBALS

// Port of Nvidia's HBAO implementation from the DX11 samples
// https://developer.nvidia.com/dx11-samples

layout (std140) uniform HBAOGlobals
{
    vec2 u_fullResolution;
    vec2 u_invFullResolution;
    
    vec2 u_aoResolution;
    vec2 u_invAOResolution;
    
    vec2 u_focalLen;
    vec2 u_invFocalLen;

    vec2 u_uvToViewA;
    vec2 u_uvToViewB;

    float u_r;
    float u_r2;
    float u_negInvR2;
    float u_maxRadiusPixels;

    float u_angleBias;
    float u_tanAngleBias;
    float u_strength;
    float u_dummy;

    vec2 u_noiseTexScale;
    vec2 u_ndcDepthConv;
};

#endif // HBAO_GLOBALS