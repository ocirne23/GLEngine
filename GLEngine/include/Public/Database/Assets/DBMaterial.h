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

	const eastl::string& getName() const      { return m_name; }

private:
	/*	
	// See Assimp aiMaterial
	enum class EBlendMode { DEFAULT = 0x0, ADDITIVE = 0x1 };
	enum class EShadingMode { FLAT = 0x1, GOURAUD = 0x2, PHONG = 0x3, BLINN = 0x4, TOON = 0x5, OREN_NAYAR = 0x6, MINNAERT = 0x7, COOK_TORRANCE = 0x8, NONE = 0x9, FRESNEL = 0xA };
	enum class ETextureFlags { INVERT = 0x1, USE_ALPHA = 0x2, IGNORE_ALPHA = 0x4 };
	enum class ETextureWrap { WRAP = 0x0, CLAMP = 0x1, MIRROR = 0x2, DECAL = 0x3 };
	enum class ETextureMapping { UV = 0x0, SPHERE = 0x1, CYLINDER = 0x2, BOX = 0x3, PLANE = 0x4, OTHER = 0x5 };
	enum class ETextureOp { MULTIPLY = 0x0, ADD = 0x1, SUBSTRACT = 0x2, DIVIDE = 0x3, SMOOTH_ADD = 0x4, SIGNED_ADD = 0x5 };
	enum class ETextureType { NONE = 0x0, DIFFUSE = 0x1, SPECULAR = 0x2, AMBIENT = 0x3, EMISSIVE = 0x4, HEIGHT = 0x5, NORMALS = 0x6, SHININESS = 0x7, OPACITY = 0x8, DISPLACEMENT = 0x9, LIGHTMAP = 0xA, REFLECTION = 0xB, UNKNOWN = 0xC };
	*/

private:

	eastl::string m_name;
	eastl::array<DBAtlasRegion, ETexTypes_COUNT> m_atlasRegions;
};
