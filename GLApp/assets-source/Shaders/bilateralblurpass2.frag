#version 330 core

uniform sampler2D u_ssaoDepthTex;
uniform sampler2D u_colorTex;

uniform vec2 u_texSize;

in vec2 v_texcoord;

layout (location = 0) out vec4 out_color;

void main()
{
	const float cPhotometricExponent = 0.5;
    const int kernelWidth = 13;
    float sigma = (kernelWidth - 1) / 6; // make the kernel span 6 sigma
	float stepX = 1.0 / u_texSize.y;
    
    float fragmentDepth = texture2D(u_ssaoDepthTex, v_texcoord).g;

    float weights = 0;
    float blurred = 0;
    
    for (float i = -(kernelWidth - 1) / 2; i < (kernelWidth - 1) / 2; i++)
    {
        float geometricWeight = exp(-pow(i, 2) / (2 * pow(sigma, 2)));

		vec2 sampleCoord = v_texcoord;
		sampleCoord.y -= i * stepX;

        vec2 accessabilityDepth = texture2D(u_ssaoDepthTex, sampleCoord).rg;
        float photometricWeight = 1 / pow((1 + abs(fragmentDepth - accessabilityDepth.g)), cPhotometricExponent);

        weights += (geometricWeight * photometricWeight);
        blurred += accessabilityDepth.r * geometricWeight * photometricWeight;
    }

    blurred /= weights;

	vec3 color = texture(u_colorTex, v_texcoord).rgb;
	color *= (1.0 - blurred);

	out_color = vec4(vec3(color), 1.0);
}