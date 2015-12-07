#ifndef CLUSTERED_SHADING
#define CLUSTERED_SHADING

/* USAGE /*
Use the FOR_LIGHT_ITERATOR(LIGHT, VSDEPTH) macro like a regular for loop, LIGHT being the the name of the iterated light, VSDEPTH being the depth of the current fragment. e.g:
FOR_LIGHT_ITERATOR(light, v_position.z)
{ // operate with light.positionRange (vec4) and light.colorIntensity (vec4)
}
*/

struct Light
{
	vec4 positionRange;
	vec4 colorIntensity;
};

layout (binding = LIGHT_INDICE_TEXTURE_BINDING_POINT) uniform isamplerBuffer u_lightIndices;
layout (binding = LIGHT_GRID_TEXTURE_BINDING_POINT)   uniform isamplerBuffer u_lightGrid;

#define FOR_LIGHT_ITERATOR(LIGHT, VSDEPTH) \
 	Light LIGHT; \
	for (ivec2 _lightIterator = _getLightListBeginEnd(VSDEPTH); _lightIteratorCheck(_lightIterator, LIGHT); ++_lightIterator.x) \

ivec2 _getLightListBeginEnd(float viewspaceDepth)
{
	ivec2 tileXY = ivec2(int(gl_FragCoord.x) / u_tileWidth, int(gl_FragCoord.y) / u_tileHeight);
	float tileZ = log(-viewspaceDepth * u_recNear) * u_recLogSD1;
	int offset = (tileXY.x * u_gridHeight + tileXY.y) * u_gridDepth + int(tileZ);
	return texelFetch(u_lightGrid, offset).xy;
}

bool _lightIteratorCheck(ivec2 iterator, out Light light)
{
	int lightIndex = texelFetch(u_lightIndices, iterator.x).r;
	light.positionRange = u_lightPositionRanges[lightIndex];
	light.colorIntensity = u_lightColorIntensities[lightIndex];
	return iterator.x < iterator.y;
}
#endif // CLUSTERED_SHADING