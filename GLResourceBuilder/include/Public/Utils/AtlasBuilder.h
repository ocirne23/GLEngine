#pragma once

#include <vector>

class DBAtlasTexture;
class DBMaterial;

class AtlasBuilder
{
public:
	static std::vector<DBAtlasTexture> createAtlases(std::vector<DBMaterial>& materials, const std::string& baseAssetPath);
};