#ifndef MATERIAL_LIGHTING
#define MATERIAL_LIGHTING
////////////////////////// MATERIAL LIGHTING //////////////////////////

/* USAGE /*
* Use 'getMaterial(uint materialID)' to retrieve a MaterialProperty object 
* for the current fragment given a materialID vertex attribute.
* Use 'vec4 getDiffuseSample(MaterialProperty material, vec2 texcoord)'
* and 'vec4 getNormalSample(MaterialProperty material, vec2 texcoord)'
* to sample a texture of the material at the texture coordionate
*/

layout (binding = DIFFUSE_ARRAY_BINDING_POINT) uniform sampler2DArray u_diffuseAtlasArray;
layout (binding = NORMAL_ARRAY_BINDING_POINT) uniform sampler2DArray u_normalAtlasArray;
layout (binding = METALNESS_ARRAY_BINDING_POINT) uniform sampler2DArray u_metalnessAtlasArray;
layout (binding = ROUGHNESS_ARRAY_BINDING_POINT) uniform sampler2DArray u_roughnessAtlasArray;
layout (binding = OPACITY_ARRAY_BINDING_POINT) uniform sampler2DArray u_opacityAtlasArray;

float _getMipMapLevel(vec2 texCoord)
{
    vec2 dx_vtc = dFdx(texCoord);
    vec2 dy_vtc = dFdy(texCoord);
    float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
    return 0.5 * log2(delta_max_sqr);
}

vec4 _sampleAtlasArray(sampler2DArray sampler, vec3 coords, vec4 texMapping)
{
	// We have to manually calculate the mipmap level because using fract() throws off
	// the default mipmap calculation
	vec2 mipmapTexCoord = coords.xy * texMapping.zw + texMapping.xy;
	float mipmapLevel = _getMipMapLevel(mipmapTexCoord * textureSize(sampler, 0).xy);
	
	vec3 texCoord = vec3(fract(coords.xy) * texMapping.zw + texMapping.xy, coords.z);
    return textureLod(sampler, texCoord, mipmapLevel);
}

MaterialProperty getMaterial(uint materialID)
{
	return u_materialProperties[materialID];
}

bool hasDiffuseTexture(MaterialProperty material)
{
	return material.diffuseAtlasNr != -1;
}

bool hasNormalTexture(MaterialProperty material)
{
	return material.normalAtlasNr != -1;
}

bool hasMetalnessTexture(MaterialProperty material)
{
	return material.metalnessAtlasNr != -1;
}

bool hasRoughnessTexture(MaterialProperty material)
{
	return material.roughnessAtlasNr != -1;
}

bool hasOpacityTexture(MaterialProperty material)
{
	return material.opacityAtlasNr != -1;
}

vec3 getDiffuseSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_diffuseAtlasArray, vec3(texcoord, material.diffuseAtlasNr), material.diffuseTexMapping).rgb;
}

vec3 getNormalSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_normalAtlasArray, vec3(texcoord, material.normalAtlasNr), material.normalTexMapping).rgb;
}

float getMetalnessSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_metalnessAtlasArray, vec3(texcoord, material.metalnessAtlasNr), material.metalnessTexMapping).r;
}

float getRoughnessSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_roughnessAtlasArray, vec3(texcoord, material.roughnessAtlasNr), material.roughnessTexMapping).r;
}

float getOpacitySample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_opacityAtlasArray, vec3(texcoord, material.opacityAtlasNr), material.opacityTexMapping).r;
}

#endif // MATERIAL_LIGHTING