#version 330 core

const uint TEXTURE_ID_BITS = 10u;
const uint POSITION_BITS_1 = 5u; // sqrt(chunkSize) + 1
const uint POSITION_BITS_2 = POSITION_BITS_1 * 2u;
const uint POSITION_BITS_3 = POSITION_BITS_1 * 3u;
const uint POSITION_AND_TEXTURE_BITS = POSITION_BITS_3 + TEXTURE_ID_BITS;
const uint POSITION_BITS_BITMASK_OFFSET = ((1u << POSITION_BITS_1) - 1u);

const uint POSITION_BITS_MASK = 0x00007FFFu;
const uint TEXTURE_ID_MASK = 0x01FF8000u;
const uint UNUSED_BITS_MASK = 0xFE000000u;

const float AO_MULT = (1.0 / 128.0);

layout (std140) uniform PerFrameData
{
	mat4 u_mvp;
	vec4 u_camPos;		//xyz used
	vec4 u_fogColor;	//rgb used
	float u_fogStart;
	float u_fogEnd;
	float padding;
	float padding2;
};

layout (std140) uniform PerChunkData
{
	vec4 u_chunkOffset;	//xyz used
};

// Bit layout: u = unused, i = textureid, x/y/z = pos within chunk //
// uuuuuuuiiiiiiiiiizzzzzyyyyyxxxxx	//
layout(location = 0) in uint in_point;
layout(location = 1) in vec2 in_texCoord;

out VS_OUT
{
	vec3 texCoord;
	float vertexAO;
	float eyeDist;
} out_vs;

void main(void)
{
	// grab the position bits from the data
	uint index = in_point & POSITION_BITS_MASK;

	// extract the position using black magic
	float z = float(index >> POSITION_BITS_2);
	float y = float((index >> POSITION_BITS_1) & POSITION_BITS_BITMASK_OFFSET);
	float x = float(index & POSITION_BITS_BITMASK_OFFSET);

	//extract textureId using black magic
	float textureId = float((in_point & TEXTURE_ID_MASK) >> POSITION_BITS_3);

	float ao = float ((in_point & UNUSED_BITS_MASK) >> POSITION_AND_TEXTURE_BITS);
	out_vs.vertexAO = 1.0 - (ao * AO_MULT);

	vec3 position = vec3(x, y, z) + u_chunkOffset.xyz;

	// offset the vertices by the position
	gl_Position = u_mvp * vec4(position, 1.0);

	out_vs.eyeDist = length(position - u_camPos.xyz); 

	// supply 3d texcoord to fragment shader
	out_vs.texCoord = vec3(in_texCoord, textureId);
}