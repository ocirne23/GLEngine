#include "Utils/AtlasBuilder.h"

#include "Utils/stb_image.h"
#include <numeric>

BEGIN_UNNAMED_NAMESPACE()

enum { 
	SUPPORT_NPOT         = false, 
	ATLAS_MAX_WIDTH      = 4096, 
	ATLAS_MAX_HEIGHT     = 4096, 
	ATLAS_NUM_COMPONENTS = 3, 
	ATLAS_START_WIDTH    = 256, 
	ATLAS_START_HEIGHT   = 256, 
	ATLAS_INCREMENT      = 256, 
	ATLAS_NUM_MIPMAPS    = 4 
};

bool isPowerOfTwo(uint x)
{
	return ((x != 0) && !(x & (x - 1)));
}

// if i is already PoT, return the next higher PoT.
uint getNextPOT(uint i)
{
	if (isPowerOfTwo(i))
		i++;

	i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;
	return i;
}


/* Increment the width/height, returns if incrementing was possible (not at max size) */
bool getNextAtlasSize(uint& width, uint& height)
{
	if (width >= ATLAS_MAX_WIDTH && height >= ATLAS_MAX_HEIGHT)
		return false;

	if (SUPPORT_NPOT)
	{
		if (width < height)
			width += ATLAS_INCREMENT;
		else
			height += ATLAS_INCREMENT;

		// Make even
		if (width % 2 != 0)
			width++;
		if (height % 2 != 0)
			height++;
	}
	else
	{
		if (width < height)
			width = getNextPOT(width);
		else
			height = getNextPOT(height);
	}

	width = std::min((int) width, (int) ATLAS_MAX_WIDTH);
	height = std::min((int) height, (int) ATLAS_MAX_HEIGHT);
	
	return true;
}

void increaseAtlasesSize(std::vector<AtlasTexture>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
	{
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS));
		return;
	}
	uint width = a_atlases[0]->m_width;
	uint height = a_atlases[0]->m_height;
	bool canIncrement = getNextAtlasSize(width, height);
	if (canIncrement)
	{	// resize all atlases to the new size
		for (TextureAtlas* a : a_atlases)
			a->initialize(width, height, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS);
	}
	else
	{	// Add one new atlas and reset all atlas sizes back to smallest (not efficient)
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS));
		for (TextureAtlas* a : a_atlases)
			a->initialize(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS);
	}
}

bool containTexturesInAtlases(std::vector<AtlasTexture*>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
		return false;
	for (TextureAtlas* a : a_atlases)
		a->clear();

	for (uint i = 0; i < a_textureInfoList.size(); ++i)
	{
		AtlasTexture* tex = a_textureInfoList[i];
		bool contained = false;
		for (uint j = 0; j < a_atlases.size(); ++j)
		{
			TextureAtlas* atlas = a_atlases[j];
			AtlasPosition pos = atlas->getRegion(tex->width, tex->height);
			if (pos.atlasIdx != -1)
			{
				tex->atlasPosition = pos;
				contained = true;
				break;
			}
		}
		if (!contained)
			return false;
	}
	return true;
}

END_UNNAMED_NAMESPACE()

std::vector<TextureAtlas*> AtlasBuilder::fitTextures(std::vector<AtlasTexture*>& a_textures)
{
	std::vector<TextureAtlas*> atlases;
	if (a_textures.size())
		while (!containTexturesInAtlases(a_textures, atlases))
			increaseAtlasesSize(a_textures, atlases); // Allocates atlases
	return atlases;
}

void AtlasBuilder::fillAtlases(std::vector<TextureAtlas*>& a_atlases, std::vector<AtlasTexture*>& a_textures)
{
	for (const AtlasTexture& tex : a_textures)
	{
		TextureAtlas* atlas = a_atlases[tex.atlasPosition.atlasIdx];

		int width, height, numComponents;
		const unsigned char* data = stbi_load(tex.filePath.c_str(), &width, &height, &numComponents, atlas->m_numComponents);
		if (!data)
		{
			printf("FAILED TO LOAD IMAGE: %s \n", tex.filePath.c_str());
			assert(false);
			return;
		}
		atlas->setRegion(tex.atlasPosition.xPos,
						 tex.atlasPosition.yPos,
						 tex.atlasPosition.width,
						 tex.atlasPosition.height,
						 data,
						 atlas->m_numComponents);
		delete data;
	}
}
