#pragma once

#include "Database/Assets/IAsset.h"
#include "Database/Assets/DBAtlasRegion.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

struct aiMaterial;

class DBMaterial : public IAsset
{
public:

	DBMaterial() {}
	DBMaterial(const aiMaterial& assimpMaterial);
	virtual ~DBMaterial() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::MATERIAL; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	const eastl::string& getDiffuseTexturePath() const;
	const eastl::string& getNormalTexturePath() const;
	
	const DBAtlasRegion& getDiffuseRegion() const { return m_diffuseRegion; }
	const DBAtlasRegion& getNormalRegion() const { return m_normalRegion; }
	float getRoughness() const { return m_roughness; }
	float getMetalness() const { return m_metalness; }
	const glm::vec3& getVertexColor() const { return m_vertexColor; }

	void setDiffuseAtlasRegion(const DBAtlasRegion& region);
	void setNormalAtlasRegion(const DBAtlasRegion& region);

private:

	void setDiffuseTexturePath(const eastl::string& filePath);
	void setNormalTexturePath(const eastl::string& filePath);

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

	DBAtlasRegion m_diffuseRegion;
	DBAtlasRegion m_normalRegion;
	glm::vec3 m_vertexColor;
	float m_roughness;
	float m_metalness;
	/*
	EBlendMode m_blendmode;
	EShadingMode m_shadingmode;
	ETextureFlags m_textureFlag;
	ETextureWrap m_textureWrap;
	ETextureMapping m_textureMapping;
	ETextureOp m_textureOp;
	*/
};
