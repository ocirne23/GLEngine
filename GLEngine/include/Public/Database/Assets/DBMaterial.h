#pragma once

#include "Database/Assets/IAsset.h"
#include "Database/Assets/DBAtlasRegion.h"
#include "EASTL/string.h"
#include "EASTL/array.h"

#include <glm/glm.hpp>

struct aiMaterial;

class DBMaterial : public IAsset
{
public:

	enum ETexTypes { ETexTypes_Diffuse = 0, ETexTypes_Normal, ETexTypes_Metalness, ETexTypes_Roughness, ETexTypes_Opacity, ETexTypes_COUNT};

	DBMaterial() {}
	DBMaterial(const aiMaterial& assimpMaterial);
	virtual ~DBMaterial() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::MATERIAL; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	void setRegion(ETexTypes type, const DBAtlasRegion& region) { m_atlasRegions[type] = region; }
	const DBAtlasRegion& getRegion(ETexTypes type) const        { return m_atlasRegions[type]; }
	const eastl::string& getTexturePath(ETexTypes type) const   { return m_atlasRegions[type].m_filePath; }
	bool hasTexture(ETexTypes type) const                       { return !m_atlasRegions[type].m_filePath.empty(); }
	const eastl::string& getName() const                        { return m_name; }

private:

	eastl::string m_name;
	eastl::array<DBAtlasRegion, ETexTypes_COUNT> m_atlasRegions;
};
