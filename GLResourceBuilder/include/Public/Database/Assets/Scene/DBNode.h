#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"

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

	rde::string m_name;
	float m_transform[3 * 3];
	uint m_parentIdx = 0;
	rde::vector<uint> m_childIndices;
	rde::vector<uint> m_meshIndices;
};