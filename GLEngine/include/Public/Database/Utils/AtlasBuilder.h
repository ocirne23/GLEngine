#pragma once

#include "EASTL/vector.h"
#include "EASTL/string.h"

#include "Database/Assets/DBMaterial.h"

class DBAtlasTexture;

class AtlasBuilder
{
public:
	static eastl::array<eastl::vector<DBAtlasTexture>, DBMaterial::ETexTypes_COUNT> createAtlases(eastl::vector<DBMaterial>& materials, const eastl::string& baseAssetPath);
};