#pragma once

#include "Database/Assets/AtlasTexture.h"
#include "Database/AssetDatabase.h"

#include <string>

class TextureLoader
{
public:
	AtlasTexture* addTexture(const std::string& filePath, AssetDatabase& assetDatabase);

private:
};

