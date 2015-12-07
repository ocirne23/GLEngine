#ifndef CLUSTERED_SHADING
#define CLUSTERED_SHADING
////////////////////////// CLUSTERED SHADING //////////////////////////

/* USAGE /*
* Use the FOR_LIGHT_ITERATOR(LIGHT, VSDEPTH) macro
* like a regular for loop, LIGHT being the the name of
* the iterated light, VSDEPTH being the depth of the current fragment
* e.g. v_position.z
* 
* FOR_LIGHT_ITERATOR(light, v_position.z)
* {
*	// operate with light.positionRange (vec4) and light.colorIntensity (vec4)
* }
*/

/* REQUIRED DEFINES /*
* MAX_LIGHTS							int
* LIGHT_GRID_WIDTH						int
* LIGHT_GRID_HEIGHT						int
* LIGHT_GRID_DEPTH						int
* LIGHT_GRID_TILE_WIDTH					int
* LIGHT_GRID_TILE_HEIGHT				int
* LIGHT_POSITION_RANGES_BINDING_POINT	int
* LIGHT_COLOR_INTENSITIES_BINDING_POINT int
*/

/* REQUIRED UNIFORMS /*
* LightPositionRanges	UBO
* LightColors 			UBO
* u_lightIndices 		isamplerBuffer
* u_lightGrid 			isamplerBuffer 
* u_recNear 			float
* u_recLogSD1 			float
*/

struct Light
{
	vec4 positionRange;
	vec4 colorIntensity;
};

uniform isamplerBuffer u_lightIndices;
uniform isamplerBuffer u_lightGrid;

#define FOR_LIGHT_ITERATOR(LIGHT, VSDEPTH) \
 	Light LIGHT; \
	for (ivec2 _lightIterator = _getLightListBeginEnd(VSDEPTH); _lightIteratorCheck(_lightIterator, LIGHT); ++_lightIterator.x) \

ivec2 _getLightListBeginEnd(float viewspaceDepth)
{
	ivec2 tileXY = ivec2(int(gl_FragCoord.x) / LIGHT_GRID_TILE_WIDTH, int(gl_FragCoord.y) / LIGHT_GRID_TILE_HEIGHT);
	float tileZ = log(-viewspaceDepth * u_recNear) * u_recLogSD1;
	int offset = (tileXY.x * LIGHT_GRID_HEIGHT + tileXY.y) * LIGHT_GRID_DEPTH + int(tileZ);
	return texelFetch(u_lightGrid, offset).xy;
}
Light _getLight(int iterator)
{
	int lightIndex = texelFetch(u_lightIndices, iterator).r;
	Light light;
	light.positionRange = u_lightPositionRanges[lightIndex];
	light.colorIntensity = u_lightColorIntensities[lightIndex];
	return light;
}

bool _lightIteratorCheck(ivec2 iterator, out Light light)
{
	light = _getLight(iterator.x);
	return iterator.x < iterator.y;
}
#endif // CLUSTERED_SHADING