#include "Database/Utils/AtlasBuilder.h"

#include "Database/Assets/DBAtlasRegion.h"
#include "Database/Assets/DBAtlasTexture.h"
#include "Database/Assets/DBMaterial.h"
#include "Database/Utils/MaxRectsPacker.h"
#include "EASTL/algorithm.h"
#include "EASTL/hash_set.h"

BEGIN_UNNAMED_NAMESPACE()

enum { 
	ATLAS_MAX_WIDTH      = 8192, 
	ATLAS_MAX_HEIGHT     = 8192, 
	ATLAS_NUM_MIPMAPS    = 4
};

glm::vec4 getTextureMapping(uint a_atlasWidth, uint a_atlasHeight, const glm::vec4& a_atlasPos)
{
	const float xOffset = a_atlasPos.x / float(a_atlasWidth);
	const float yOffset = a_atlasPos.y / float(a_atlasHeight);
	const float width = a_atlasPos.z / float(a_atlasWidth);
	const float height = a_atlasPos.w / float(a_atlasHeight);
	return glm::vec4(xOffset, yOffset, width, height);
}

END_UNNAMED_NAMESPACE()

/** Packs all the textures of the given set of materials into atlases, different texture types are in different atlasses, there can be more than one atlas page per type. */
eastl::array<eastl::vector<DBAtlasTexture>, DBMaterial::ETexTypes_COUNT> AtlasBuilder::createAtlases(eastl::vector<DBMaterial>& a_materials, const eastl::string& a_baseAssetPath)
{
	eastl::hash_set<eastl::string> texFiles[DBMaterial::ETexTypes_COUNT];
	DBAtlasRegion reg;

	for (const DBMaterial& mat : a_materials)
		for (DBMaterial::ETexTypes i = DBMaterial::ETexTypes_Diffuse; i < DBMaterial::ETexTypes_COUNT; i = DBMaterial::ETexTypes(i + 1))
			if (mat.hasTexture(i)) texFiles[i].insert(mat.getTexturePath(i));
	
	eastl::vector<Rect> rects[DBMaterial::ETexTypes_COUNT];
	eastl::vector<DBAtlasRegion> regions[DBMaterial::ETexTypes_COUNT];

	for (DBMaterial::ETexTypes i = DBMaterial::ETexTypes_Diffuse; i < DBMaterial::ETexTypes_COUNT; i = DBMaterial::ETexTypes(i + 1))
	{
		uint rectIdx = 0;
		for (const eastl::string& tex : texFiles[i])
		{
			DBAtlasRegion reg;
			reg.loadInfo(a_baseAssetPath + tex);
			regions[i].push_back(reg);
			rects[i].push_back(Rect(rectIdx++, 0, 0, reg.m_texWidth, reg.m_texHeight));
		}
	}

	eastl::array<eastl::vector<DBAtlasTexture>, DBMaterial::ETexTypes_COUNT> atlasTextures;
	const uint numComponentsForType[DBMaterial::ETexTypes_COUNT] = {
		3, // Diffuse
		3, // Normal
		1, // Metalness
		1, // Roughness
		1  // Opacity
	};

	MaxRectsPacker::Settings packerSettings;
	packerSettings.maxWidth = ATLAS_MAX_WIDTH;
	packerSettings.maxHeight = ATLAS_MAX_HEIGHT;
	packerSettings.paddingPx = ATLAS_NUM_MIPMAPS * ATLAS_NUM_MIPMAPS;
	MaxRectsPacker packer(packerSettings);

	for (DBMaterial::ETexTypes i = DBMaterial::ETexTypes_Diffuse; i < DBMaterial::ETexTypes_COUNT; i = DBMaterial::ETexTypes(i + 1))
	{
		if (rects[i].empty())
			continue;

		eastl::vector<Page> pages = packer.pack(rects[i]);
		print("type: %i, numPages: %i\n", i, pages.size());
		for (Page& page : pages) 
			print("numImages: %i, size: %i - %i\n", page.rects.size(), page.width, page.height);

		atlasTextures[i].reserve(pages.size());
		const uint atlasWidth = pages[0].width;
		const uint atlasHeight = pages[0].height;

		for (uint j = 0; j < pages.size(); ++j)
		{
			const Page& page = pages[j];
			uint numComponents = numComponentsForType[i];
			atlasTextures[i].emplace_back(atlasWidth, atlasHeight, numComponents, ATLAS_NUM_MIPMAPS);
			DBAtlasTexture& tex = atlasTextures[i].back();

			for (const Rect& rect : page.rects)
			{
				DBAtlasRegion& region = regions[i][rect.id];
				region.m_atlasPosition = glm::uvec4(rect.x, rect.y, rect.width, rect.height);
				region.m_atlasMapping = getTextureMapping(atlasWidth, atlasHeight, region.m_atlasPosition);
				region.m_atlasIdx = j;
				tex.writeRegionTexture(region);

				for (DBMaterial& mat : a_materials)
					if (a_baseAssetPath + mat.getTexturePath(i) == region.m_filePath)
						mat.setRegion(i, region);
			}
		}
	}

	return atlasTextures;
}
