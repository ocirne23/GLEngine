#include "Shaders/globals.glsl"
#include "Shaders/clusteredshading.glsl"
#include "Shaders/material.glsl"
#include "Shaders/Shadow/pcf.glsl"

in vec3 v_rawPos;
in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
flat in uint v_materialID;
in vec4 v_shadowCoord;

layout (location = 0) out vec3 out_color;

layout (binding = DFV_TEXTURE_BINDING_POINT) uniform sampler2D u_dfvTexture;

vec3 rotateNormal(vec3 normal)
{
	normal = normal * 2.0 - 1.0;
	vec3 N = v_normal;
    vec3 dp1 = dFdx(v_position);
    vec3 dp2 = dFdy(v_position);
    vec2 duv1 = dFdx(v_texcoord);
    vec2 duv2 = dFdy(v_texcoord);
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    mat3 rotMat = mat3(T * invmax, B * invmax, N);
	return normalize(rotMat * normal);
}

float inverseSquareFalloff(float lightDistance, float lightRange)
{
	return pow(max(1.0 - pow((lightDistance / lightRange), 4), 0), 2) / (pow(lightDistance, 2) + 1);
}

// [Burley 2012, "Physically-Based Shading at Disney"]
vec3 diffuseBurley(vec3 diffuse, float Roughness, float NoV, float NoL, float VoH)
{
	float FD90 = VoH * VoH * Roughness;
	float FdV = 1 + (FD90 - 1) * exp2( (-5.55473 * NoV - 6.98316) * NoV );
	float FdL = 1 + (FD90 - 1) * exp2( (-5.55473 * NoL - 6.98316) * NoL );
	return diffuse / PI * FdV * FdL;
}

// [Gotanda 2012, "Beyond a Simple Physically Based Blinn-Phong Model in Real-Time"]
vec3 diffuseOrenNayar( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
	float VoL = 2 * VoH - 1;
	float m = Roughness * Roughness;
	float m2 = m * m;
	float C1 = 1 - 0.5 * m2 / (m2 + 0.33);
	float Cosri = VoL - NoV * NoL;
	float C2 = 0.45 * m2 / (m2 + 0.09) * Cosri * ( Cosri >= 0 ? min( 1, NoL / NoV ) : NoL );
	return DiffuseColor / PI * ( NoL * C1 + C2 );
}

// [http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/]
float specularGGX(float roughness, float F0, float NoH, float HoL, float NoL)
{
	float D = texture(u_dfvTexture, vec2(NoH * NoH * NoH * NoH, roughness)).x;
	vec2 FVhelper = texture(u_dfvTexture, vec2(HoL, roughness)).yz;
	float FV = F0 * FVhelper.x + FVhelper.y;
	return D * FV * NoL;
}

vec3 doLight(vec3 lightContrib, vec3 L, vec3 N, vec3 V, float NdotV, float F0, vec3 diffuse, float smoothness, float metalness)
{
	vec3 H = normalize(L + V);
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float HdotL = clamp(dot(H, L), 0.0, 1.0);
	float VdotH = clamp(dot(V, H), 0.0, 1.0);
#if 1
	vec3 diffuseContrib = diffuseOrenNayar(diffuse, smoothness, NdotV, NdotL, VdotH);
#else
	vec3 diffuseContrib = diffuseBurley(diffuse, smoothness, NdotV, NdotL, VdotH);
#endif
	float specularContrib = specularGGX(smoothness, F0, NdotH, HdotL, NdotL);
	return lightContrib * diffuseContrib + lightContrib * specularContrib;
}

void main()
{
	MaterialProperty material = getMaterial(v_materialID);
	if (material.color.a < 0.4 && material.color.a > 0.1) // Hacky transparency for IFC demo
		discard;

	vec3 diffuse = hasDiffuseTexture(material) ? getDiffuseSample(material, v_texcoord).rgb : material.color.rgb;
	vec3 N       = hasNormalTexture(material) ?  rotateNormal(getNormalSample(material, v_texcoord).rgb) : v_normal;
	vec3 V       = normalize(u_eyePos - v_position);
	float NdotV  = clamp(dot(N, V), 0.0, 1.0);

	float smoothness = material.smoothness;
	float metalness  = material.metalness;
	float albedo     = (diffuse.r + diffuse.g + diffuse.b) / 3.0;
	float F0         = mix(0.035, albedo, metalness);

	vec3 lightAccum = vec3(0.0);
	FOR_LIGHT_ITERATOR(light, v_position.z)
	{
		vec3 L = light.positionRange.xyz - v_position;
		float lightDistance = length(L);
		L /= lightDistance;
		float attenuation = inverseSquareFalloff(lightDistance, light.positionRange.w);
		vec3 lightContrib = light.colorIntensity.rgb * light.colorIntensity.a * PI * attenuation;
		lightAccum += doLight(lightContrib, L, N, V, NdotV, F0, diffuse, smoothness, metalness);
	}
	float visibility = pcf(v_shadowCoord);
	if (visibility > 0.0)
	{
		vec3 sunContrib = u_sunColorIntensity.rgb * u_sunColorIntensity.a * PI * visibility;
		lightAccum += visibility * doLight(sunContrib, u_sunDir, N, V, NdotV, F0, diffuse, smoothness, metalness);
	}
	lightAccum += diffuse * u_ambient;
	out_color = vec3(lightAccum);
}
// out_color = texture(u_textureAtlasArray, vec3(gl_FragCoord.xy / vec2(1200, 720), 0)).rgb; // Visualize atlas