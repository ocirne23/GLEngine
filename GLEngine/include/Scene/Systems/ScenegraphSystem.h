#pragma once

#include "Scene/Systems/System.h"
#include "rde/vector.h"
#include <glm/glm.hpp>

class ScenegraphSystem : public System
{
public:
	ScenegraphSystem();
	~ScenegraphSystem();

	void reserve(uint count);
	void update(float deltaSec) OVERRIDE;

private:

	struct Node
	{
		glm::mat4 worldSpacePosition;
		glm::mat4 transform;
		byte numChildren;
		bool dirty;
	};

	void transform(uint& it, const Node& parent);

	rde::vector<Node> m_nodes;
};