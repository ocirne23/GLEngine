#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

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

private:

	eastl::string m_name;
	float m_transform[3 * 3];
	uint m_parentIdx = 0;
	eastl::vector<uint> m_childIndices;
	eastl::vector<uint> m_meshIndices;
};