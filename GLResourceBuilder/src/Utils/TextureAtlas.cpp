#include "TextureAtlas.h"

#include <assert.h>
#include <memory>

TextureAtlas::TextureAtlas(int width, int height, int numComponents)
	: m_width(width), m_height(height), m_numComponents(numComponents)
{
	m_root.left = m_root.right = 0;
	m_root.x = m_root.y = 0;
	m_root.width = width;
	m_root.height = height;

	m_data = new unsigned char[m_width * m_height * m_numComponents];
}

TextureAtlas::~TextureAtlas()
{
	delete[] m_data;
}

TextureAtlas::AtlasRegion TextureAtlas::getRegion(int width, int height)
{
	Node* node = getRegion(&m_root, width, height);
	if (node)
		return{ node->x, node->y, node->width, node->height };
	else
		return { 0, 0, 0, 0 };
}

TextureAtlas::Node* TextureAtlas::getRegion(TextureAtlas::Node *node, int width, int height)
{
	if (node->left || node->right)
	{
		if (node->left)
		{
			Node* newNode = getRegion(node->left, width, height);
			if (newNode)
				return newNode;
		}
		if (node->right)
		{
			Node* newNode = getRegion(node->right, width, height);
			if (newNode)
				return newNode;
		}
		return 0;
	}

	if (width > node->width || height > node->height)
		return 0;

	int w = node->width - width;
	int h = node->height - height;
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

void TextureAtlas::setRegion(int x, int y, int width, int height, const unsigned char* data, int stride)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < (m_width));
	assert((x + width) <= (m_width));
	assert(y < (m_height));
	assert((y + height) <= (m_height));
	assert(stride == m_numComponents);

	if (width == m_width && height == m_height)
	{
		memcpy(m_data, data, width * height * m_numComponents);
	}
	else
	{
		int depth = m_numComponents;
		for (int i = 0; i < height; ++i)
		{
			memcpy(m_data + ((y + i) * m_width + x) * m_numComponents,
				data + (i * width * stride), width * m_numComponents);
		}
	}
}