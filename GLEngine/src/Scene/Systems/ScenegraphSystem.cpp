#include "Scene/Systems/ScenegraphSystem.h"

void ScenegraphSystem::transform(uint& a_it, const Node& a_parent)
{
	for (uint i = 0; i < a_parent.numChildren; ++i)
	{
		Node& child = m_nodes[++a_it];
		if (child.dirty)
		{
			child.worldSpacePosition = a_parent.worldSpacePosition * child.transform;
			child.dirty = false;
		}
		transform(a_it, child);
	}
}

void ScenegraphSystem::update(float /*a_deltaSec*/)
{
	for (uint i = 0; i < m_nodes.size(); ++i)
	{
		Node& node = m_nodes[i];
		node.worldSpacePosition = node.transform;
		node.dirty = false;

		transform(i, node);
	}
}

void ScenegraphSystem::reserve(uint a_count)
{
	m_nodes.reserve(a_count);
}