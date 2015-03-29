#version 330 core

uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;

uniform vec2 u_nearFar;
uniform vec2 u_texSize;

in vec2 v_texcoord;

layout (location = 0) out vec4 out_normalDepth;

float linearizeDepth(float depth)
{
	float n = u_nearFar.x;
	float f = u_nearFar.y;
	float z = depth;

	// float linearDepth = (2.0 * n) / (f + n – (z * (f – n)));	// why won't this compile?!

	float what = 2.0 * n;
	float the = f + n;
	float fuck = z * (f - n);

	float linearDepth = what / (the - fuck);

	return linearDepth;
}

void main()
{
	ivec2 texPos = ivec2(v_texcoord * u_texSize);
	ivec2 fetchPos = ivec2(texPos * 2 + ivec2((texPos.y & 1) ^ 1, (texPos.x & 1) ^ 1));

	vec3 normal = texelFetch(u_normalTex, fetchPos, 0).rgb;
	float depth = linearizeDepth(texelFetch(u_depthTex, fetchPos, 0).r);

	out_normalDepth = vec4(normal, depth);
}