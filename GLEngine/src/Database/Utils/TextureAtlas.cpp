#include "Database/Utils/TextureAtlas.h"

#include <assert.h>

void TextureAtlas::initialize(uint a_width, uint a_height, uint a_numComponents, uint a_numMipMaps, uint a_atlasIndex)
{
	assert(a_width % 2 == 0 && a_height % 2 == 0 && "Atlas width and height must be divideable by 2");
	
	m_width = a_width;
	m_height = a_height;
	m_numComponents = a_numComponents;
	m_padding = a_numMipMaps * a_numMipMaps;
	m_numMipMaps = a_numMipMaps;
	m_atlasIndex = a_atlasIndex;

	SAFE_DELETE(m_root.left);
	SAFE_DELETE(m_root.right);

	m_root.x = m_root.y = 0;
	m_root.width = a_width;
	m_root.height = a_height;
}

void TextureAtlas::clear()
{
	initialize(m_width, m_height, m_numComponents, m_numMipMaps, m_atlasIndex);
}

glm::uvec4 TextureAtlas::getRegion(uint width, uint height)
{
	if (width > m_width || height > m_height)
		return {0, 0, 0, 0};
	if (!m_root.left && !m_root.right && (width == m_root.width && height == m_root.height))
	{	// if no region has been used yet and region size is equal to atlas size, use entire atlas
		m_root.left = new Node();
		m_root.right = new Node();
		return {0, 0, width, height};
	}
	const Node* node = getRegion(&m_root, width + m_padding * 2, height + m_padding * 2);
	if (node)
		return {node->x + m_padding, node->y + m_padding, width, height};
	else
		return {0, 0, 0, 0};
}

const TextureAtlas::Node* TextureAtlas::getRegion(TextureAtlas::Node *node, uint width, uint height)
{
	if (node->left || node->right)
	{
		if (node->left)
		{
			const Node* newNode = getRegion(node->left, width, height);
			if (newNode)
				return newNode;
		}
		if (node->right)
		{
			const Node* newNode = getRegion(node->right, width, height);
			if (newNode)
				return newNode;
		}
		return NULL;
	}
	if (width > node->width || height > node->height)
		return NULL;

	const uint w = node->width - width;
	const uint h = node->height - height;
	node->left = new Node();
	node->right = new Node();
	if (w <= h)
	{
		node->left->x = node->x + width;
		node->left->y = node->y;
		node->left->width = w;
		node->left->height = height;

		node->right->x = node->x;
		node->right->y = node->y + height;
		node->right->width = node->width;
		node->right->height = h;
	}
	else
	{
		node->left->x = node->x;
		node->left->y = node->y + height;
		node->left->width = width;
		node->left->height = h;

		node->right->x = node->x + width;
		node->right->y = node->y;
		node->right->width = w;
		node->right->height = node->height;
	}
	node->width = width;
	node->height = height;
	return node;
}