#include "Scene/Systems/StaticScenegraphSystem.h"

void StaticScenegraphSystem::transform(uint& a_it, const Node& a_parent)
{
	const uint numChildren = a_parent.flags & Node::NUM_CHILDREN_MASK;
	for (uint i = 0; i < numChildren; ++i)
	{
		Node& child = m_nodes[++a_it];
		if ((child.flags & Node::DIRTY_FLAG_MASK) != 0) // check dirty flag set
		{
			child.worldSpacePosition = a_parent.worldSpacePosition * child.transform;
			child.flags &= ~Node::DIRTY_FLAG_MASK; // clear dirty flag
		}
		transform(a_it, child);
	}
}

void StaticScenegraphSystem::update(float /*a_deltaSec*/)
{
	for (uint i = 0; i < (uint) m_nodes.size(); ++i)
	{
		Node& node = m_nodes[i];
		node.worldSpacePosition = node.transform;
		node.flags &= ~Node::DIRTY_FLAG_MASK; // clear dirty flag

		transform(i, node);
	}
}

void StaticScenegraphSystem::reserve(uint a_count)
{
	m_nodes.reserve(a_count);
}

uint StaticScenegraphSystem::addNode(uint a_maxHierarchySize, uint a_parent)
{
	if (a_parent == NO_PARENT)
	{
		const int idx = m_nodes.size();
		m_nodes.resize(idx + a_maxHierarchySize);
		m_nodes[idx].flags |= Node::INITIALIZED_FLAG_MASK;
		m_nodes[idx].flags |= (a_maxHierarchySize << 16);
		return idx;
	}
	else 
	{
		// Find first free spot within max hierarchy size
		const Node& parent = m_nodes[a_parent];
		const int maxHierarchySize = (parent.flags & Node::MAX_HIERARCHY_SIZE_MASK) >> 16;
		for (uint i = a_parent; i < a_parent + maxHierarchySize; ++i)
		{
			Node& child = m_nodes[i];

			if ((child.flags & Node::INITIALIZED_FLAG_MASK) == 0)
			{
				if (i + a_maxHierarchySize > a_parent + maxHierarchySize)
				{	// Child does not fit in parent hierarchy
					return 0xFFFFFFFF;
				}
				else
				{
					m_nodes[i].flags |= Node::INITIALIZED_FLAG_MASK;
					m_nodes[i].flags |= (a_maxHierarchySize << 16);
				}
			}
			else
			{
				int childMaxHierarchySize = (child.flags & Node::MAX_HIERARCHY_SIZE_MASK) >> 16;
				i += childMaxHierarchySize;
			}
		}
	}
}

void StaticScenegraphSystem::setTransform(uint a_node, const glm::quat& a_transform)
{
	m_nodes[a_node].flags |= Node::DIRTY_FLAG_MASK;
	m_nodes[a_node].transform = a_transform;
}

void StaticScenegraphSystem::setTransform(uint a_node, const glm::mat4& a_transform)
{
	m_nodes[a_node].flags |= Node::DIRTY_FLAG_MASK;
	m_nodes[a_node].transform = glm::quat_cast(a_transform);
}

const glm::quat& StaticScenegraphSystem::getWorldspacePosition(uint a_node)
{
	return m_nodes[a_node].worldSpacePosition;
}

const glm::mat4& StaticScenegraphSystem::getWorldspacePositionMatrix(uint a_node)
{
	return glm::mat4_cast(m_nodes[a_node].worldSpacePosition);
}