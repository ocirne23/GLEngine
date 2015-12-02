#pragma once

#include "EASTL/vector.h"
#include "EASTL/string.h"

class DBAtlasTexture;
class DBMaterial;

class AtlasBuilder
{
public:
	static eastl::vector<DBAtlasTexture> createAtlases(eastl::vector<DBMaterial>& materials, const eastl::string& baseAssetPath);
};