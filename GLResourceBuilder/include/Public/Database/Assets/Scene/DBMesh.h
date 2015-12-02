#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include "Utils/Vec.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

#include <assimp/scene.h>

class DBMesh : public IAsset
{
public:

	DBMesh() {}
	DBMesh(const aiMesh& assimpMesh);
	virtual ~DBMesh() {}

	const eastl::string& getName() { return m_name; }
	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::MESH; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

private:

	struct Vertex
	{
		Vertex() {}
		Vertex(aiVector3D& a_position, aiVector3D& a_texcoords, aiVector3D& a_normal, aiVector3D& a_tangents, aiVector3D& a_bitangents, uint a_materialID)
			: position((Vec3&) a_position), texcoords((Vec2&) a_texcoords), normal((Vec3&) a_normal)
			, tangents((Vec3&) a_tangents), bitangents((Vec3&) a_bitangents), materialID(a_materialID)
		{}
		Vec3 position;
		Vec2 texcoords;
		Vec3 normal;
		Vec3 tangents;
		Vec3 bitangents;
		uint materialID;
	};

private:

	eastl::string m_name;
	eastl::vector<Vertex> m_vertices;
	eastl::vector<uint> m_indices;
};