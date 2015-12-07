#pragma once

#include "Core.h"
#include "Database/Assets/IAsset.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

#include <glm/glm.hpp>

struct aiNode;

class DBNode : public IAsset
{
public:

	DBNode() {}
	DBNode(const aiNode& assimpNode, uint parentIdx);
	virtual ~DBNode() {}

	void addChild(uint childIdx);

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::NODE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	const eastl::string& getName() const               { return m_name; }
	const glm::mat4& getTransform() const              { return m_transform; }
	uint getParentIdx() const                          { return m_parentIdx; }
	const eastl::vector<uint>& getChildIndices() const { return m_childIndices; }
	const eastl::vector<uint>& getMeshIndices() const  { return m_meshIndices; }

private:

	eastl::string m_name;
	glm::mat4 m_transform;
	uint m_parentIdx = 0;
	eastl::vector<uint> m_childIndices;
	eastl::vector<uint> m_meshIndices;
};