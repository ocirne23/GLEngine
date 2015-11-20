#include "Utils/TextureLoader.h"

#include <assert.h>
#include <assimp/scene.h>


AtlasTexture* TextureLoader::addTexture(const std::string& filePath, AssetDatabase& assetDatabase)
{
	AtlasTexture* texture = new AtlasTexture();
	texture->filePath = filePath;
	int result = stbi_info(filePath.c_str(), &texture->width, &texture->height, &texture->numComp);
	texture->loaded = (result != 0);
	assetDatabase.addAsset(filePath, texture);
	return texture;
}
