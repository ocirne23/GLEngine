#pragma once

#include "Core.h"
#include "Database/Assets/IAsset.h"
#include "Database/Assets/DBAtlasTexture.h"
#include "Database/Assets/DBMaterial.h"
#include "Database/Assets/DBMesh.h"
#include "Database/Assets/DBNode.h"
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
	DBScene(const eastl::string& sceneFilePath);
	virtual ~DBScene() {}

	void mergeMeshes();
	void swapMaterial(const eastl::string& matName, const eastl::string& withMatName);

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::SCENE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	const eastl::vector<DBNode>& getNodes() const                 { return m_nodes; }
	const eastl::vector<DBMesh>& getMeshes() const                { return m_meshes; }
	const eastl::vector<DBMaterial>& getMaterials() const         { return m_materials; }
	const eastl::vector<DBAtlasTexture>& getAtlasTextures() const { return m_atlasTextures; }

	eastl::vector<DBNode>& getNodes()                 { return m_nodes; }
	eastl::vector<DBMesh>& getMeshes()                { return m_meshes; }
	eastl::vector<DBMaterial>& getMaterials()         { return m_materials; }
	eastl::vector<DBAtlasTexture>& getAtlasTextures() { return m_atlasTextures; }

	const uint numNodes() const         { return m_nodes.size(); }
	const uint numMeshes() const        { return m_meshes.size(); }
	const uint numMaterials() const     { return m_materials.size(); }
	const uint numAtlasTextures() const { return m_atlasTextures.size(); }

private:

	void mergeMeshes(DBMesh& mergedMesh, DBNode& node, const glm::mat4& parentTransform);

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