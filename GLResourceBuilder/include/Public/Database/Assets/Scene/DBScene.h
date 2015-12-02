#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include "Database/Assets/Scene/DBAtlasTexture.h"
#include "Database/Assets/Scene/DBMaterial.h"
#include "Database/Assets/Scene/DBMesh.h"
#include "Database/Assets/Scene/DBNode.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"

struct aiScene;
struct aiNode;
class DBAtlasRegion;
class DBAtlasTexture;
class DBMesh;
class DBMaterial;
class DBNode;
class TextureAtlas;

class DBScene : public IAsset
{
public:

	DBScene() {}
	DBScene(const aiScene& assimpScene, const eastl::string& baseAssetPath);
	virtual ~DBScene() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::SCENE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

private:

	uint processNodes(const aiNode* assimpNode, uint parentIdx);

private:
	// Scene graph
	eastl::vector<DBNode> m_nodes;
	// Geometry info
	eastl::vector<DBMesh> m_meshes;
	// Material/texture info
	eastl::vector<DBMaterial> m_materials;
	// Multiple atlas textures containing all textures in the scene
	eastl::vector<DBAtlasTexture> m_atlasTextures;
};