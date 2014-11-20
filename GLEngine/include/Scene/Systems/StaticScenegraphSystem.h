#pragma once

#include "Scene/Systems/System.h"
#include "rde/vector.h"
#include <glm/glm.hpp>

class StaticScenegraphSystem : public System
{
public:
	static const uint NO_PARENT = 0xFFFFFFFF;
public:
	StaticScenegraphSystem();
	~StaticScenegraphSystem();

	void reserve(uint count);
	void update(float deltaSec) OVERRIDE;
	
	uint addNode(uint maxHierarchySize, uint parent = NO_PARENT);
	void setTransform(uint node, const glm::mat4& transform);
	const glm::mat4& getWorldspacePosition(uint node);

private:

	struct Node
	{
		static const uint EMPTY_NODE = 0x00000000;
		static const uint NUM_CHILDREN_MASK			= 0x000000FF; // 1-8
		static const uint DIRTY_FLAG_MASK			= 0x00000100; // 9
		static const uint INITIALIZED_FLAG_MASK		= 0x00000200; // 10
		static const uint IS_CHILD_FLAG_MASK			= 0x00000400; // 11
		static const uint MAX_HIERARCHY_SIZE_MASK	= 0xFFFF0000; // 16-32

		glm::mat4 worldSpacePosition;
		glm::mat4 transform;
		uint flags;
	};

	void transform(uint& it, const Node& parent);

	rde::vector<Node> m_nodes;
};