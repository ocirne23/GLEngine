#include "Utils/AtlasBuilder.h"

#include "Utils/TextureAtlas.h"
#include "BuilderCore.h"
#include <numeric>

BEGIN_UNNAMED_NAMESPACE()

enum { SUPPORT_NPOT = false, ATLAS_MAX_WIDTH = 4096, ATLAS_MAX_HEIGHT = 4096, ATLAS_NUM_COMPONENTS = 3, ATLAS_START_WIDTH = 256, ATLAS_START_HEIGHT = 256, ATLAS_INCREMENT = 256 };

struct AtlasTextureInfo
{
	struct TextureInfo
	{
		std::string filePath;
		int width   = 0;
		int height  = 0;
		int numComp = 0;
		int result  = 0;
		int index   = -1;
	} textureInfo;

	struct AtlasInfo
	{
		uint atlasIdx = 0;
		TextureAtlas::AtlasRegion region;
	} atlasInfo;
};

uint getNextPOT(uint i)
{
	i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;
	return i;
}

AtlasTextureInfo addTextureInfo(std::vector<AtlasTextureInfo>& a_textureInfoList, aiTextureType a_textureType, const aiMaterial* a_material, const std::string& a_basePath)
{
	std::string texturePath;
	if (a_material->GetTextureCount(a_textureType))
	{
		aiString path;
		a_material->GetTexture(a_textureType, 0, &path);
		if (path[0] == '*')
		{
			print("EMBEDDED TEXTURE: %s\n", path.C_Str());
		}
		texturePath = a_basePath + path.C_Str();
	} 
	else
	{
		assert(false);
		return AtlasTextureInfo(); // Material doesn't have texture of type
	}
	
	// If texture already exists, return existing.
	for (uint i = 0; i < a_textureInfoList.size(); ++i)
		if (a_textureInfoList[i].textureInfo.filePath == texturePath)
			return a_textureInfoList[i];

	// If texture doesnt exist, get the info and add to end
	AtlasTextureInfo info;
	info.textureInfo.result = stbi_info(info.textureInfo.filePath, &info.textureInfo.width, &info.textureInfo.height, &info.textureInfo.numComp);
	if (info.textureInfo.result)
	{
		info.textureInfo.index = a_textureInfoList.size();
		a_textureInfoList.push_back(info);
	}
	assert(info.textureInfo.result && "Failed to load texture");
	return info;
}

void addDiffuseTextureInfo(std::vector<AtlasTextureInfo>& a_textureInfoList, const aiMaterial* a_material, const std::string& a_basePath)
{
	addTextureInfo(a_textureInfoList, aiTextureType_DIFFUSE, a_material, a_basePath);
}

void addNormalTextureInfo(const aiMaterial* a_material, const std::string& a_basePath)
{
	if (a_material->GetTextureCount(aiTextureType_NORMALS))
		addTextureInfo(aiTextureType_NORMALS, a_material, a_basePath);
	else if (a_material->GetTextureCount(aiTextureType_HEIGHT))
		addTextureInfo(aiTextureType_HEIGHT, a_material, a_basePath);  // .obj files have their normals map under aiTextureType_HEIGHT
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

	width = std::min(width, (int) ATLAS_MAX_WIDTH);
	height = std::min(height, (int) ATLAS_MAX_HEIGHT);
	
	return true;
}

void increaseAtlasesSize(std::vector<AtlasTextureInfo>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
	{
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS));
		return;
	}
	uint width = a_atlases[0].m_width;
	uint height = a_atlases[0].m_height;
	bool canIncrement = getNextAtlasSize(width, height);
	if (canIncrement)
	{	// resize all atlases to the new size
		for (TextureAtlas* a : a_atlases)
			a->initialize(width, height, ATLAS_NUM_COMPONENTS);
	}
	else
	{	// Add one new atlas and reset all atlas sizes back to smallest (not efficient)
		a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS));
		for (TextureAtlas* a : a_atlases)
			a->initialize(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, ATLAS_NUM_COMPONENTS);
	}
}

bool containTexturesInAtlases(std::vector<AtlasTextureInfo>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
		return false;
	for (TextureAtlas* a : a_atlases)
		a->clear();

	for (uint i = 0; i < a_textureInfoList.size(); ++i)
	{
		AtlasTextureInfo& tex = a_textureInfoList[i];
		bool contained = false;
		for (uint j = 0; j < a_atlases.size(); ++j)
		{
			TextureAtlas* atlas = a_atlases[j];
			TextureAtlas::AtlasRegion region = atlas->getRegion(tex.textureInfo.width, tex.textureInfo.height);
			if (region.width && region.height)
			{
				tex.atlasInfo.atlasIdx = j;
				tex.atlasInfo.region = region;
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

std::vector<AtlasTextureInfo> AtlasBuilder::getTextures(const aiScene* a_scene)
{
	print("NUM EMBEDDED TEXTURES: %i\n", a_scene->mNumTextures());
	std::vector<AtlasTextureInfo> textureInfoList;
	for (uint i = 0; i < a_scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = a_scene->mMaterials[i];
		addDiffuseTextureInfo(textureInfoList, material);
		addNormalTextureInfo(textureInfoList, material);
	}

	return textureInfoList;
}

std::vector<TextureAtlas*> AtlasBuilder::fitMaterials(std::vector<AtlasTextureInfo> a_textures)
{
	std::vector<TextureAtlas*> atlases;
	if (a_textures.size())
		while (!containTexturesInAtlases(a_textures, atlases))
			increaseAtlasesSize(a_textures, atlases); // Allocates atlases
	return atlases;
}

void AtlasBuilder::fillAtlases(std::vector<TextureAtlas*>& a_atlases, std::vector<AtlasTextureInfo>& a_textures, const char* a_baseScenePath)
{
	const std::string baseScenePath(a_baseScenePath);
	for (const AtlasTextureInfo& tex : a_textures)
	{
		TextureAtlas* atlas = a_atlases[tex.atlasInfo.atlasIdx];
		tex.atlasInfo.region;

		int width, height, numComponents;
		const unsigned char* data = stbi_load(baseScenePath + tex.textureInfo.filePath.c_str(), &width, &height, &numComponents, atlas->m_numComponents);
		if (!data)
		{
			printf("FAILED TO LOAD IMAGE: %s \n", tex.textureInfo.filePath.c_str());
			assert(false);
			return;
		}
		atlas->setRegion(tex.atlasInfo.region.x,
						 tex.atlasInfo.region.y,
						 tex.atlasInfo.region.width,
						 tex.atlasInfo.region.height,
						 data,
						 atlas->m_numComponents);
		delete data;
	}
}
