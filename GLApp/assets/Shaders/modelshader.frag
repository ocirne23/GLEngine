#include "Shaders/clusteredshading.txt"

////////////////////////// IN / OUT //////////////////////////

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec4 out_color;

////////////////////////// MATERIAL LIGHTING //////////////////////////

const uint MAX_MATERIALS = 409u;
const uint INVALID_MATERIAL = 0xFFFFFFFFu;
const float DEFAULT_SPECULAR = 0.2;
const float SPECULAR_POWER = 1.2;
const float ATTENUATION = 0.004;
const float BUMP_MAP_STRENGTH = 1.2;

struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 bumpTexMapping;
	vec4 specTexMapping;
	vec4 maskTexMapping;
	int diffuseAtlasNr;
	int bumpAtlasNr;
	int specularAtlasNr;
	int maskAtlasNr;
};

uniform sampler2DArray u_1cTextureArray;
uniform sampler2DArray u_3cTextureArray;

vec3 sample3CAtlasArray(int atlasNr, vec4 texMapping, vec2 uv)
{
	return texture(u_3cTextureArray, vec3(fract(uv) * texMapping.zw + texMapping.xy, atlasNr)).rgb;	
}

float sample1CAtlasArray(int maskAtlasNr, vec4 maskTexMapping, vec2 uv)
{
	return texture(u_1cTextureArray, vec3(fract(uv) * maskTexMapping.zw + maskTexMapping.xy, maskAtlasNr)).r;
}

layout (std140) uniform MaterialProperties
{
	MaterialProperty u_materialProperties[MAX_MATERIALS];
};

vec3 getBumpedNormal(vec3 normalSample)
{
	vec3 normal = normalSample * 2.0 - 1;
	mat3 rotMat = mat3(v_tangent, v_bitangent, v_normal);
	return rotMat * vec3(normal);
}

////////////////////////// MAIN //////////////////////////

#define PI 3.1415926535

uniform vec3 u_eyePos;
uniform vec3 u_ambient;
uniform sampler2D u_dfvTexture;

float G1V(float dotNV, float k)
{
	return 1.0 / (dotNV * (1.0 - k ) + k);
}

vec2 GGX_FV_helper(float HdotL, float roughnessSquared)
{
	float OneMinusHdotLPow5 = exp2(-8.65617024533378044416 * HdotL); //F == pow(1.0 - HdotL, 5);
	
	float alpha = roughnessSquared;
	float k = alpha / 2.0;
	float k2 = k * k;
	float invK2 = 1.0 - k2;
	float V = 1.0 / (HdotL * HdotL * invK2 + k2);

	return vec2((1.0 - OneMinusHdotLPow5) * V, OneMinusHdotLPow5 * V);
}

float GGX_D(float NdotH, float roughnessSquared)
{
	float alpha = roughnessSquared;
	float alphaSqr = alpha * alpha;
	float denom = NdotH * NdotH * (alphaSqr - 1.0) + 1.0;
	
	float D = alphaSqr / (PI * denom * denom);
	return D;
}

// [Burley 2012, "Physically-Based Shading at Disney"]
vec3 Diffuse_Burley( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
	float FD90 = 0.5 + 2 * VoH * VoH * Roughness;
	float FdV = 1 + (FD90 - 1) * exp2( (-5.55473 * NoV - 6.98316) * NoV );
	float FdL = 1 + (FD90 - 1) * exp2( (-5.55473 * NoL - 6.98316) * NoL );
	return DiffuseColor / PI * FdV * FdL;
}

// [Gotanda 2012, "Beyond a Simple Physically Based Blinn-Phong Model in Real-Time"]
vec3 Diffuse_OrenNayar( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
	float VoL = 2 * VoH - 1;
	float m = Roughness * Roughness;
	float m2 = m * m;
	float C1 = 1 - 0.5 * m2 / (m2 + 0.33);
	float Cosri = VoL - NoV * NoL;
	float C2 = 0.45 * m2 / (m2 + 0.09) * Cosri * ( Cosri >= 0 ? min( 1, NoL / NoV ) : NoL );
	return DiffuseColor / PI * ( NoL * C1 + C2 );
}

void doLightGGX(out vec3 diffuseContrib, out vec3 specularContrib, vec3 diffuse, float roughness, float F0, vec3 P, vec3 N, vec3 V, Light light)
{
	vec3 lightPos = light.positionRange.xyz;
	float lightRange = light.positionRange.w;
	vec4 lightColor = light.color;

	vec3 L = lightPos - P;
	float dist = length(L);
	L /= dist;
	vec3 H = normalize(L + V);
	
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float HdotL = clamp(dot(H, L), 0.0, 1.0);
	float NdotV = clamp(dot(N, V), 0.0, 1.0);
	float VdotH = clamp(dot(V, H), 0.0, 1.0);
	
	float atten = max(1.0 - ((dist * dist) / (lightRange * lightRange)), 0.0);
	
	float D = texture(u_dfvTexture, vec2(NdotH*NdotH*NdotH*NdotH, roughness)).x; // == GGX_D(NdotH, roughnessSquared);
	vec2 FVhelper = texture(u_dfvTexture, vec2(HdotL, roughness)).yz; 			// == GGX_FV_helper(HdotL, roughnessSquared);
	
	float FV = F0 * FVhelper.x + FVhelper.y;
	
	vec3 lightContrib = lightColor.rgb * atten;

	specularContrib = lightContrib * (D * FV * NdotL);
	diffuseContrib = lightContrib * Diffuse_OrenNayar(diffuse, roughness, NdotV, NdotL, VdotH);
}


struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 bumpTexMapping;
	vec4 specTexMapping;
	vec4 maskTexMapping;
	int diffuseAtlasNr;
	int bumpAtlasNr;
	int specularAtlasNr;
	int maskAtlasNr;
};

void main()
{
	MaterialProperty material = u_materialProperties[v_materialID];
	
	if (material.maskAtlasNr != -1 && sample1CAtlasArray(material.maskAtlasNr, material.maskTexMapping, v_texcoord) < 0.5)
	{
		discard;
	}
	
	vec3 diffuse = sample3CAtlasArray(material.diffuseAtlasNr, material.diffuseTexMapping, v_texcoord);
	vec3 normal;
	if (material.bumpAtlasNr != -1)
	{
		normal = getBumpedNormal(sample3CAtlasArray(material.bumpAtlasNr, material.bumpTexMapping, v_texcoord));
	}
	else
	{
		normal = v_normal;
	}
	float specular = sample1CAtlasArray(material.specularAtlasNr, material.specTexMapping, v_texcoord);

	vec3 diffuseAccum = vec3(0);
	vec3 specularAccum = vec3(0);
	
	vec3 N = normalize(normal);
	vec3 V = normalize(u_eyePos - v_position);
	
	FOR_LIGHT_ITERATOR(light, v_position.z)
	{
		float roughness = clamp(specular, 0.01, 0.99);
		float albedo = (diffuse.r + diffuse.g + diffuse.b) / 3.0;
		float F0 = mix(0.04, albedo, roughness); //replace roughness with metalness //0.04; // refraction index;

		vec3 diffuseContrib, specularContrib;
		doLightGGX(diffuseContrib, specularContrib, diffuse, roughness, F0, v_position, N, V, light);
		
		diffuseAccum += diffuseContrib;
		specularAccum += specularContrib;
	}
	
	diffuseAccum += diffuse * u_ambient;
	out_color = vec4(diffuseAccum + specularAccum, 1.0);
	//out_color = vec4(diffuse, 1.0);
	//out_color = vec4(vec3(normal), 1.0) + vec4(diffuseAccum + specularAccum, 1.0) * 0.00000000001;
}