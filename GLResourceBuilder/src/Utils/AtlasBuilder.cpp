#include "Utils/AtlasBuilder.h"

#include "Database/Assets/Scene/DBAtlasRegion.h"
#include "Database/Assets/Scene/DBAtlasTexture.h"
#include "Database/Assets/Scene/DBMaterial.h"
#include "Utils/AtlasPosition.h"
#include "Utils/TextureAtlas.h"
#include "Utils/stb_image.h"

#include <algorithm>
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

	width = std::min((int) width, (int) ATLAS_MAX_WIDTH);
	height = std::min((int) height, (int) ATLAS_MAX_HEIGHT);
	
	return true;
}

void increaseAtlasesSize(std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
	{
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPMAPS, 0));
		return;
	}
	uint width = a_atlases[0]->m_width;
	uint height = a_atlases[0]->m_height;
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

bool containTexturesInAtlases(std::unordered_map<std::string, DBAtlasRegion>& a_regions, std::vector<TextureAtlas*>& a_atlases)
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
			AtlasPosition pos = atlas->getRegion(reg.m_width, reg.m_height);
			if (pos.isValid())
			{
				reg.m_atlasPosition = pos;
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

std::vector<DBAtlasTexture> AtlasBuilder::createAtlases(std::vector<DBMaterial>& a_materials, const std::string& a_baseAssetPath)
{
	// First we create an unique set of regions so we dont have multiple AtlasRegion instances for the same texture
	std::unordered_map<std::string, DBAtlasRegion> uniqueRegions;
	for (const DBMaterial& mat : a_materials) 
	{
		const std::string& diffuseFilePath = mat.getDiffuseTexturePath();
		if (uniqueRegions.find(diffuseFilePath) == uniqueRegions.end())
		{
			DBAtlasRegion reg;
			reg.loadInfo(a_baseAssetPath + "\\" + diffuseFilePath);
			uniqueRegions.insert({diffuseFilePath, reg});
		}

		const std::string& normalFilePath = mat.getNormalTexturePath();
		if (uniqueRegions.find(normalFilePath) == uniqueRegions.end())
		{
			DBAtlasRegion reg;
			reg.loadInfo(a_baseAssetPath + "\\" + normalFilePath);
			uniqueRegions.insert({normalFilePath, reg});
		}
	}
	
	// Then we fit these textures into atlases (not the image data, just fitting rectangles)
	std::vector<TextureAtlas*> atlases;
	while (!containTexturesInAtlases(uniqueRegions, atlases))
		increaseAtlasesSize(atlases);

	// Update all the materials so they correctly reference the regions
	for (DBMaterial& mat : a_materials)
	{
		mat.setDiffuseTextureRegion(uniqueRegions.find(mat.getDiffuseTexturePath())->second);
		mat.setNormalTextureRegion(uniqueRegions.find(mat.getNormalTexturePath())->second);
	}

	// Create AtlasTexture objects out of the TextureAtlases (which are IAssets)
	std::vector<DBAtlasTexture> atlasTextures;
	atlasTextures.reserve(atlases.size());
	for (TextureAtlas* atlas : atlases)
	{
		atlasTextures.emplace_back(atlas->m_width, atlas->m_width, atlas->m_numComponents, atlas->m_numMipMaps);
		SAFE_DELETE(atlas);
	}
	atlases.clear();

	// Write the image data of every region into the atlas textures
	for (auto& pair : uniqueRegions)
		atlasTextures[pair.second.m_atlasPosition.atlasIdx].setRegion(pair.second);

	return atlasTextures; // rvo
}
