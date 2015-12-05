#include "Database/Utils/AtlasBuilder.h"

#include "Database/Assets/DBAtlasRegion.h"
#include "Database/Assets/DBAtlasTexture.h"
#include "Database/Assets/DBMaterial.h"
#include "Database/Utils/AtlasPosition.h"
#include "Database/Utils/TextureAtlas.h"
#include "Database/Utils/stb_image.h"
#include "EASTL/algorithm.h"
#include "EASTL/hash_map.h"

#include <glm/glm.hpp>

#include <numeric>

BEGIN_UNNAMED_NAMESPACE()

enum { 
	ATLAS_MAX_WIDTH      = 4096, 
	ATLAS_MAX_HEIGHT     = 4096, 
	ATLAS_NUM_COMPONENTS = 3, 
	ATLAS_START_WIDTH    = 64, 
	ATLAS_START_HEIGHT   = 64, 
	ATLAS_NUM_MIPMAPS    = 4 
};

glm::vec4 getTextureMapping(uint a_atlasWidth, uint a_atlasHeight, const AtlasPosition& a_atlasPos)
{
	const float xOffset = a_atlasPos.xPos / (float) a_atlasWidth;
	const float yOffset = a_atlasPos.yPos / (float) a_atlasHeight;
	const float width = a_atlasPos.width / (float) a_atlasWidth;
	const float height = a_atlasPos.height / (float) a_atlasHeight;
	return glm::vec4(xOffset, yOffset, width, height);
}

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

	if (width < height)
		width = getNextPOT(width);
	else
		height = getNextPOT(height);

	width = eastl::min((int) width, (int) ATLAS_MAX_WIDTH);
	height = eastl::min((int) height, (int) ATLAS_MAX_HEIGHT);
	
	return true;
}

void increaseAtlasesSize(eastl::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
	{
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS, 0));
		return;
	}
	uint width = a_atlases[0]->getWidth();
	uint height = a_atlases[0]->getHeight();
	bool canIncrement = getNextAtlasSize(width, height);
	if (canIncrement)
	{	// resize all atlases to the new size
		for (uint i = 0; i < a_atlases.size(); ++i)
			a_atlases[i]->initialize(width, height, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS, i);
	}
	else
	{	// Add one new atlas and reset all atlas sizes back to smallest (not efficient)
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS, (uint) a_atlases.size()));
		for (uint i = 0; i < a_atlases.size(); ++i)
			a_atlases[i]->initialize(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS, i);
	}
}

/* Returns if the AltasRegions fit in the TextureAtlases, sets the AltasPosition of each AltasRegion */
bool containTexturesInAtlases(eastl::hash_map<eastl::string, DBAtlasRegion>& a_regions, eastl::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
		return false;
	for (TextureAtlas* a : a_atlases)
		a->clear();

	for (auto& pair: a_regions)
	{
		DBAtlasRegion& reg = pair.second;
		bool contained = false;
		for (TextureAtlas* atlas : a_atlases)
		{
			AtlasPosition pos = atlas->getRegion(reg.m_texWidth, reg.m_texHeight);
			if (pos.isValid())
			{
				reg.m_atlasPosition = pos;
				reg.m_atlasMapping = getTextureMapping(atlas->getWidth(), atlas->getHeight(), pos);
				reg.m_atlasIdx = atlas->getAtlasIdx();
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

eastl::vector<DBAtlasTexture> AtlasBuilder::createAtlases(eastl::vector<DBMaterial>& a_materials, const eastl::string& a_baseAssetPath)
{
	// First we create an unique set of regions so we don't have multiple AtlasRegions for the same texture
	eastl::hash_map<eastl::string, DBAtlasRegion> uniqueRegions;
	for (const DBMaterial& mat : a_materials) 
	{
		const eastl::string& diffuseFilePath = mat.getDiffuseTexturePath();
		if (uniqueRegions.find(diffuseFilePath) == uniqueRegions.end())
		{
			DBAtlasRegion reg;
			reg.loadInfo(a_baseAssetPath + "\\" + diffuseFilePath);
			uniqueRegions.insert({diffuseFilePath, reg});
		}

		const eastl::string& normalFilePath = mat.getNormalTexturePath();
		if (uniqueRegions.find(normalFilePath) == uniqueRegions.end())
		{
			DBAtlasRegion reg;
			reg.loadInfo(a_baseAssetPath + "\\" + normalFilePath);
			uniqueRegions.insert({normalFilePath, reg});
		}
	}
	
	// Then we fit these textures into atlases (not the image data, just fitting rectangles)
	eastl::vector<TextureAtlas*> atlases;
	while (!containTexturesInAtlases(uniqueRegions, atlases))
		increaseAtlasesSize(atlases);

	// Create AtlasTexture objects out of the TextureAtlases (which are IAssets)
	eastl::vector<DBAtlasTexture> atlasTextures;
	atlasTextures.reserve(atlases.size());
	for (TextureAtlas* atlas : atlases)
	{
		atlasTextures.push_back(DBAtlasTexture(atlas->getWidth(), atlas->getHeight(), atlas->getNumComponents(), atlas->getNumMipmaps()));
		SAFE_DELETE(atlas);
	}
	atlases.clear();

	for (auto& pair : uniqueRegions)
	{
		const DBAtlasRegion& reg = pair.second;
		// Write the image data of every region into the atlas textures
		atlasTextures[reg.m_atlasIdx].writeRegionTexture(reg);
		// Update the region in each material with the correct atlasposition
		for (DBMaterial& mat : a_materials)
		{
			if (mat.getDiffuseTexturePath() == reg.m_filePath)
				mat.setDiffuseAtlasRegion(reg);
			if (mat.getNormalTexturePath() == reg.m_filePath)
				mat.setNormalAtlasRegion(reg);
		}
	}
	return atlasTextures; // rvo
}
