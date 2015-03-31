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

/* REQUIRED UNIFORMS /*
* u_textureArray 						sampler2DArray
* u_materialProperties[MAX_MATERIALS] 	MaterialProperty (set by GLMesh)
*/

#define MAX_MATERIALS 409u

struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 normalTexMapping;
	int diffuseAtlasNr;
	int normalAtlasNr;
	int padding;
	int padding2;
};

uniform sampler2DArray u_textureArray;

layout (std140) uniform MaterialProperties
{
	MaterialProperty u_materialProperties[MAX_MATERIALS];
};

float _getMipMapLevel(vec2 texture_coordinate)
{
    vec2  dx_vtc        = dFdx(texture_coordinate);
    vec2  dy_vtc        = dFdy(texture_coordinate);
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

vec4 getDiffuseSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_textureArray, vec3(texcoord, material.diffuseAtlasNr), material.diffuseTexMapping);
}

vec4 getNormalSample(MaterialProperty material, vec2 texcoord)
{
	return _sampleAtlasArray(u_textureArray, vec3(texcoord, material.normalAtlasNr), material.normalTexMapping);
}
#endif // MATERIAL_LIGHTING