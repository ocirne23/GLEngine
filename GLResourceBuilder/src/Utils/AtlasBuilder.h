#pragma once

#include "BuilderCore.h"
#include "Database/Assets/AtlasTexture.h"
#include "Database/Assets/TextureAtlas.h"
#include <assimp/scene.h>
#include <vector>
#include <string>

class AtlasBuilder
{
public:
	static std::vector<TextureAtlas*> fitTextures(std::vector<AtlasTexture*>& textures);
	static void fillAtlases(std::vector<TextureAtlas*>& atlases, std::vector<AtlasTexture>& textures);
};