#pragma once

#include "Database/IAsset.h"
#include "Utils/Vec.h"
#include <string>

class Material : public IAsset
{
public:

	// See Assimp aiMaterial
	enum class EBlendMode { DEFAULT = 0x0, ADDITIVE = 0x1 };
	enum class EShadingMode { FLAT = 0x1, GOURAUD = 0x2, PHONG = 0x3, BLINN = 0x4, TOON = 0x5, OREN_NAYAR = 0x6, MINNAERT = 0x7, COOK_TORRANCE = 0x8, NONE = 0x9, FRESNEL = 0xA };
	enum class ETextureFlags { INVERT = 0x1, USE_ALPHA = 0x2, IGNORE_ALPHA = 0x4 };
	enum class ETextureWrap { WRAP = 0x0, CLAMP = 0x1, MIRROR = 0x2, DECAL = 0x3 };
	enum class ETextureMapping { UV = 0x0, SPHERE = 0x1, CYLINDER = 0x2, BOX = 0x3, PLANE = 0x4, OTHER = 0x5 };
	enum class ETextureOp { MULTIPLY = 0x0, ADD = 0x1, SUBSTRACT = 0x2, DIVIDE = 0x3, SMOOTH_ADD = 0x4, SIGNED_ADD = 0x5 };
	enum class ETextureType { NONE = 0x0, DIFFUSE = 0x1, SPECULAR = 0x2, AMBIENT = 0x3, EMISSIVE = 0x4, HEIGHT = 0x5, NORMALS = 0x6, SHININESS = 0x7, OPACITY = 0x8, DISPLACEMENT = 0x9, LIGHTMAP = 0xA, REFLECTION = 0xB, UNKNOWN = 0xC };

public:

	std::string diffuseTexturePath;
	std::string normalTexturePath;
	Vec3 vertexColor;
	float roughness;
	float metalness;
	EBlendMode blendmode;
	EShadingMode shadingmode;
	ETextureFlags textureFlag;
	ETextureWrap textureWrap;
	ETextureMapping textureMapping;
	ETextureOp textureOp;

public:

	virtual uint getByteSize() override;
	virtual void write(std::ostream& file) override;
	virtual EAssetType getAssetType() override { return EAssetType::MATERIAL; }
};
