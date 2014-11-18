#pragma once

#include "Scene/Systems/System.h"
#include "rde/vector.h"
#include <glm/glm.hpp>

class ScenegraphSystem : public System
{
public:
	ScenegraphSystem();
	~ScenegraphSystem();

	void update(float deltaSec) OVERRIDE;
private:

	struct Node
	{
		glm::mat4 transform;
		Node* parent;
	};

	rde::vector<Node> m_rootNodes;
};