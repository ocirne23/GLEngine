#pragma once

#include "BuilderCore.h"
#include "Database/IAsset.h"
#include <string>

class Scene : public IAsset
{
public:


	struct Node
	{
		std::string m_names;
		uint parentIdx = 0;
		uint numChildren = 0;
		uint* childIndices = NULL;
		float transform[4 * 4];
	};

public:

	virtual uint getByteSize() override;
	virtual EAssetType getAssetType() override;
	virtual void write(std::ostream& file) override;

private:

	uint m_numNodes = 0;
	Node* m_nodes   = NULL;
	
};