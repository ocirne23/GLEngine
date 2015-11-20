#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include <vector>
#include <string>

class Scene : public IAsset
{
public:

	struct Node
	{
		std::string m_name;
		float transform[4 * 4];
		uint parentIdx = 0;
		std::vector<uint> childIndices;
		std::vector<uint> meshIndices;
	};

public:

	uint addNode(std::string name, uint parentIdx);
	void addMesh(uint nodeIdx, uint meshIdx);


	virtual uint getByteSize() override;
	virtual EAssetType getAssetType() override;
	virtual void write(std::ostream& file) override;

private:

	std::vector<Node> m_nodes;
};