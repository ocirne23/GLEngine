#pragma once

#include <assimp/scene.h>
#include <vector>

class TextureAtlas;
struct AtlasTextureInfo;

class AtlasBuilder
{
public:
	/* Retrieve info for all the textures in the scene */
	static std::vector<AtlasTextureInfo> getTextures(const aiScene* scene);
	static std::vector<TextureAtlas*> fitMaterials(std::vector<AtlasTextureInfo>& textures);
	static void fillAtlases(std::vector<TextureAtlas*>& atlases, std::vector<AtlasTextureInfo>& textures, const char* baseScenePath);
};