#pragma once

#include "BuilderCore.h"
#include "Utils/TextureAtlas.h"
#include <assimp/scene.h>
#include <vector>
#include <string>

struct AtlasTextureInfo
{
	struct TextureInfo
	{
		std::string filePath;
		int width = 0;
		int height = 0;
		int numComp = 0;
		int result = 0;
		int index = -1;
	} textureInfo;

	struct AtlasInfo
	{
		uint atlasIdx = 0;
		TextureAtlas::AtlasRegion region;
	} atlasInfo;
};

class AtlasBuilder
{
public:
	/* Retrieve info for all the textures in the scene */
	static std::vector<AtlasTextureInfo> getTextures(const aiScene* scene);
	static std::vector<TextureAtlas*> fitMaterials(std::vector<AtlasTextureInfo>& textures);
	static void fillAtlases(std::vector<TextureAtlas*>& atlases, std::vector<AtlasTextureInfo>& textures, const std::string& baseScenePath);
};