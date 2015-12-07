// GLSL port of Nvidia's HBAO implementation from the DX11 samples
// https://developer.nvidia.com/dx11-samples

#include "Shaders/globals.glsl"

#define NUM_DIRECTIONS 6
#define NUM_STEPS 6
#define RANDOM_TEXTURE_WIDTH 8

#define MS_FBO
#ifdef MS_FBO
uniform sampler2DMS u_linearDepthTex;
#else
uniform sampler2D u_linearDepthTex;
#endif

uniform sampler2D u_randomTex;

in vec2 v_position;
in vec2 v_texcoord;

layout(location = 0) out vec2 out_aoDepth;

float viewSpaceZFromDepth(float a_d)
{
    a_d = a_d * 2.0 - 1.0;
    return -1.0 / (u_ndcDepthConv.x * a_d + u_ndcDepthConv.y);
}

float invLength(vec2 a_v)
{
    return inversesqrt(dot(a_v, a_v));
}

float tangent(vec3 a_p, vec3 a_s)
{
    return -(a_p.z - a_s.z) * invLength(a_s.xy - a_p.xy);
}

float tangent(vec3 a_t)
{
    return a_t.z * invLength(a_t.xy);
}

float biasedTangent(vec3 a_t)
{
    return a_t.z * invLength(a_t.xy) + u_tanAngleBias;
}

vec3 uvToEye(vec2 a_uv, float a_eyeZ)
{
    vec2 uv = u_uvToViewA * a_uv + u_uvToViewB;
    return vec3(uv * a_eyeZ, a_eyeZ);
}

vec3 fetchEyePos(vec2 a_uv)
{
#ifdef MS_FBO
	float depth = texelFetch(u_linearDepthTex, ivec2(a_uv * u_aoResolution), gl_SampleID).r;
#else
	float depth = texture(u_linearDepthTex, a_uv).r;
#endif
    float z = viewSpaceZFromDepth(depth);
    return uvToEye(a_uv, z);
}

float length2(vec3 a_v)
{
    return dot(a_v, a_v);
}

vec3 minDiff(vec3 a_p, vec3 a_pr, vec3 a_pl)
{
    vec3 v1 = a_pr - a_p;
    vec3 v2 = a_p - a_pl;
    return length2(v1) < length2(v2) ? v1 : v2; 
}

float falloff(float a_d2)
{
    return a_d2 * u_negInvR2 + 1.0;
}

vec2 snapUVOffset(vec2 a_uv)
{
    return round(a_uv * u_aoResolution) * u_invAOResolution;
}

float tanToSin(float a_x)
{
    return a_x * inversesqrt(a_x * a_x + 1.0);
}

vec3 tangentVector(vec2 a_deltaUV, vec3 a_dPdu, vec3 a_dPdv)
{
    return a_deltaUV.x * a_dPdu + a_deltaUV.y * a_dPdv;
}

vec2 rotateDirections(vec2 a_dir, vec2 a_cosSin)
{
    return vec2(a_dir.x * a_cosSin.x - a_dir.y * a_cosSin.y,
		a_dir.x * a_cosSin.y + a_dir.y * a_cosSin.x);
}

void computeSteps(inout vec2 a_stepSizeUV, inout float a_numSteps, float a_rayRadiusPx, float a_rand)
{
    a_numSteps = min(NUM_STEPS, a_rayRadiusPx);
    float stepSizePx = a_rayRadiusPx / (a_numSteps + 1);
    float maxNumSteps = u_maxRadiusPixels / stepSizePx;
    
    if (maxNumSteps < a_numSteps)
    {
	a_numSteps = floor(maxNumSteps + a_rand);
	a_numSteps = max(a_numSteps, 1);
	stepSizePx = u_maxRadiusPixels / a_numSteps;
    }
    
    a_stepSizeUV = stepSizePx * u_invAOResolution;
}

float horizonOcclusion(vec2 a_deltaUV, vec3 a_p, float a_numSteps, 
                       float a_randstep, vec3 a_dPdu, vec3 a_dPdv)
{
    float ao = 0.0;

    vec2 uv = v_texcoord + snapUVOffset(a_randstep * a_deltaUV);
    vec2 deltaUV = snapUVOffset(a_deltaUV);
    vec3 t = deltaUV.x * a_dPdu + deltaUV.y * a_dPdv;
    float tanH = biasedTangent(t);
    float sinH = tanToSin(tanH);

    for (float j = 1; j < a_numSteps; ++j)
    {
	uv += deltaUV;
	vec3 s = fetchEyePos(uv);
	float tanS = tangent(a_p, s);
	float d2 = length2(s - a_p);

	if (d2 < u_r2 && tanS > tanH)
	{
	    float sinS = tanToSin(tanS);
	    ao += falloff(d2) * (sinS - sinH);

	    tanH = tanS;
	    sinH = sinS;
	}
    }
    return ao;
}

void main()
{
	float ao = 1.0;

	vec3 p = fetchEyePos(v_texcoord);
	vec3 rand = texture(u_randomTex, v_texcoord.xy * u_noiseTexScale).rgb;
	vec2 rayRadiusUV = (0.5 * u_r * u_focalLen) / -p.z;
	float rayRadiusPx = rayRadiusUV.x * u_aoResolution.x;
	if (rayRadiusPx > 1.0)
	{
		ao = 0.0;
		float numSteps;
		vec2 stepSize;
		computeSteps(stepSize, numSteps, rayRadiusPx, rand.z);

		vec3 pr, pl, pt, pb;
		pr = fetchEyePos(v_texcoord + vec2(u_invAOResolution.x, 0));
		pl = fetchEyePos(v_texcoord + vec2(-u_invAOResolution.x, 0));
		pt = fetchEyePos(v_texcoord + vec2(0, u_invAOResolution.y));
		pb = fetchEyePos(v_texcoord + vec2(0, -u_invAOResolution.y));

		vec3 dPdu = minDiff(p, pr, pl);
		vec3 dPdv = minDiff(p, pt, pb) * (u_aoResolution.y * u_invAOResolution.x);

		float alpha = 2.0 * PI / NUM_DIRECTIONS;

		for (float d = 0.0; d < NUM_DIRECTIONS; ++d)
		{
			float angle = alpha * d;
			vec2 dir = rotateDirections(vec2(cos(angle), sin(angle)), rand.xy);
			vec2 deltaUV = dir * stepSize;
			ao += horizonOcclusion(deltaUV, p, numSteps, rand.z, dPdu, dPdv);
		}
		ao = 1.0 - ao / NUM_DIRECTIONS * u_strength;
	}
	out_aoDepth = vec2(ao, p.z);
}