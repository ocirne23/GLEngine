#version 330 core

uniform sampler2D u_normalDepthTex;

uniform vec2 u_nearFar;

/*http://www.derschmale.com/2013/12/20/an-alternative-implementation-for-hbao-2/*/
/* invFocalLen.x = tan(fovyRadians * 0.5) * aspect; invFocalLen.y = tan(fovyRadians * 0.5); */
uniform vec2 u_invFocalLen;
uniform vec2 u_texSize;

uniform mat4 u_projection;

in vec2 v_texcoord;

layout (location = 0) out vec2 out_ssaoDepth;

const float strength = 0.5;
const float falloff = 0.000005;
const float radius = 5.0;
const float threshhold = 0.005;
const int maxStepsPerRay = 5;

const int numSamples = 8;
const vec2 samples[] = vec2[](
	vec2(-0.9802384, 0.1286682),
	vec2(-0.6142182, 0.2866169),
	vec2(-0.7174929, -0.4962823),
	vec2(-0.3603347, -0.07681315),
	vec2(-0.2270383, -0.6700009),
	vec2(-0.08130036, -0.3280035),
	vec2(0.109532, -0.8490288),
	vec2(-0.03837306, 0.224186),
	vec2(0.2673156, -0.4311423),
	vec2(0.375321, 0.1444955),
	vec2(-0.6960008, -0.148204),
	vec2(-0.6986037, 0.6374375),
	vec2(0.4563469, -0.7576086),
	vec2(-0.2095659, 0.6713459),
	vec2(0.9127353, 0.2199387),
	vec2(0.8401861, -0.2427511),
	vec2(0.2101471, 0.7919461),
	vec2(0.6960869, 0.5120318),
	vec2(-0.559074, -0.7983131),
	vec2(0.7887973, -0.5803353)
);

vec3 getPosition(vec2 uv, float depth)
{
   uv = (uv * vec2(2.0, -2.0) - vec2(1.0, -1.0));
   return vec3(uv * vec2(u_invFocalLen.x, u_invFocalLen.y) * depth, depth);
}

float rand(vec2 seed) 
{
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float computeDepthSimilarity(float delta, float tweakScale)
{
	return clamp(1.0 - delta * tweakScale, 0.0, 1.0);
}

float doAO(vec2 uv, vec2 offset, float depth, vec3 normal)
{
	vec4 occNormalDepth = texture(u_normalDepthTex, uv + offset);
	vec3 occNormal = occNormalDepth.rgb;
	float occDepth = occNormalDepth.a;

	float difference = depth - occDepth;

	float similarity = computeDepthSimilarity(abs(difference), (1.0 / threshhold));

	return step(falloff, difference) * (1.0 - smoothstep(falloff, strength, difference)) * (1.0 - dot(occNormal, normal)) * similarity;
}

vec2 snapToTexel(vec2 uv)
{
	return round(uv * u_texSize) * (1.0 / u_texSize);
}

void main()
{ 
	vec4 normalDepth = texture(u_normalDepthTex, v_texcoord);

	vec3 normal = normalDepth.rgb;
	float depth = normalDepth.a;
	vec3 position = getPosition(v_texcoord, depth);

	vec2 randSeed = v_texcoord * depth;
	float randX = rand(randSeed);
	float randY = rand(randSeed * 612.1267);
	vec2 random = vec2(randX, randY);
  
	float radiusOverDepth = radius / depth / u_nearFar.y;
	float occlusion = 0.0;
	/*
	for (int i = 0; i < numSamples; ++i)
	{
		vec2 sampleDir = rotate(samples[i], random);
		
		vec2 texelSizedStep = sampleDir * (1.0 / u_texSize);
		texelSizedStep *= radiusOverDepth;

		vec3 tangent = getPosition(v_texcoord + texelSizedStep, depth);
		tangent -= dot(normal, tangent) * normal;

		vec2 stepUV = snapToTexel(sampleDir / (numStepsPerRay - 1));
		jitteredOffset = stepUV * rand(sampleDir);
		vec2 uv = snapToTexel(v_texcoord + jitteredOffset);


	}
	*/
	for (int i = 0; i < numSamples; ++i) 
	{
		vec2 ray = radiusOverDepth * reflect(samples[i].xy, random.xy);
		vec2 hemiRay = v_texcoord + sign(dot(ray.xy, normal.xy)) * ray.xy;

		vec4 sampleNormalDepth = texture(u_normalDepthTex, hemiRay);
		vec3 sampleNormal = sampleNormalDepth.rgb;
		float sampleDepth = sampleNormalDepth.a;
		vec3 samplePosition = getPosition(hemiRay, sampleDepth);

		vec3 direction = position - samplePosition;

		float difference = length(direction);
		vec3 dir = normalize(direction);

		float similarity = computeDepthSimilarity(abs(difference), (1.0 / threshhold));

		occlusion += step(falloff, difference) * (1.0 - smoothstep(falloff, strength, difference)) * (1.0 - (dot(sampleNormal, normal) + 0.2)) * similarity;
	}
	float ao = occlusion * (1.0 / (numSamples));
	out_ssaoDepth = vec2(ao, depth);
}