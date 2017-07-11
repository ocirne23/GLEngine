#pragma once

#include "Core.h"
#include "Database/Assets/IAsset.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

struct aiMesh;

class DBMesh : public IAsset
{
public:

	struct Vertex
	{
		Vertex() {}
		glm::vec3 position;
		glm::vec2 texcoords;
		glm::vec3 normal;
		glm::vec4 tangents;
		uint materialID;
	};

public:

	DBMesh() {}
	DBMesh(const aiMesh& assimpMesh);
	virtual ~DBMesh() {}

	void merge(const DBMesh& mesh, const glm::mat4& transform);

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::MESH; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	const eastl::string& getName() const             { return m_name; }
	const eastl::vector<Vertex>& getVertices() const { return m_vertices; }
	const eastl::vector<uint>& getIndices() const    { return m_indices; }
	const glm::vec3& getBoundsMin() const            { return m_boundsMin; }
	const glm::vec3& getBoundsMax() const            { return m_boundsMax; }

private:

	eastl::string m_name;
	eastl::vector<Vertex> m_vertices;
	eastl::vector<uint> m_indices;
	glm::vec3 m_boundsMin = glm::vec3(FLT_MAX);
	glm::vec3 m_boundsMax = glm::vec3(FLT_MIN);
};