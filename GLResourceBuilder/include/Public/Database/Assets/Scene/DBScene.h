#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include "Database/Assets/Scene/DBAtlasTexture.h"
#include "Database/Assets/Scene/DBMaterial.h"
#include "Database/Assets/Scene/DBMesh.h"
#include "Database/Assets/Scene/DBNode.h"

#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/vector.h"

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
	DBScene(const aiScene& assimpScene, const rde::string& baseAssetPath);
	virtual ~DBScene() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::SCENE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

private:

	uint processNodes(const aiNode* assimpNode, uint parentIdx);

private:
	// Scenegraph
	rde::vector<DBNode> m_nodes;
	// Geometry info
	rde::vector<DBMesh> m_meshes;
	// Material/texture info
	rde::vector<DBMaterial> m_materials;
	// Multiple atlas textures containing all textures in the scene
	rde::vector<DBAtlasTexture> m_atlasTextures;
};