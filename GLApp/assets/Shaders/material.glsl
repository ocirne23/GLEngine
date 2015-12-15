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

layout (binding = TEXTURE_ATLAS_ARRAY_BINDING_POINT) uniform sampler2DArray u_textureAtlasArray;

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

vec4 getDiffuseSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_textureAtlasArray, vec3(texcoord, material.diffuseAtlasNr), material.diffuseTexMapping);
}

vec4 getNormalSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_textureAtlasArray, vec3(texcoord, material.normalAtlasNr), material.normalTexMapping);
}
#endif // MATERIAL_LIGHTING