#include "TextureAtlas.h"

#include <assert.h>
#include <memory>

namespace
{
	inline void setPixel(unsigned char* data, int width, int height, int x, int y, int numComponents, const unsigned char* pixelData)
	{
		memcpy(data + ((width * y) + x) * numComponents, pixelData, numComponents);
	}

	inline void getPixel(const unsigned char* data, int width, int height, int x, int y, int numComponents, unsigned char* outPixelData)
	{
		memcpy(outPixelData, data + ((width * y) + x) * numComponents, numComponents);
	}
}

TextureAtlas::TextureAtlas(int width, int height, int numComponents, int numMipMaps)
: m_width(width), m_height(height), m_numComponents(numComponents), m_numMipMaps(numMipMaps), m_padding(numMipMaps * numMipMaps)
{
	assert(width % 2 == 0 && height % 2 == 0 && "Atlas width and height must be divideable by 2");

	m_root.left = m_root.right = 0;
	m_root.x = m_root.y = 0;
	m_root.width = width;
	m_root.height = height;

	unsigned int size = m_width * m_height * m_numComponents;

	m_data = new unsigned char[size];

	// Initialize atlas color to red
	unsigned char red[] = { 255, 0, 0, 255 };
	for (int x = 0; x < m_width; ++x)
		for (int y = 0; y < m_height; ++y)
			setPixel(m_data, m_width, m_height, x, y, m_numComponents, red);
}

TextureAtlas::~TextureAtlas()
{
	delete[] m_data;
}

TextureAtlas::AtlasRegion TextureAtlas::getRegion(int width, int height)
{
	Node* node = NULL;
	if (!m_root.left && !m_root.right && (width == m_root.width || height == m_root.height))
	{	// if no region has been used yet and region size is equal to atlas size, no need for padding
		//m_padding = 0;
	}
	
	node = getRegion(&m_root, width + m_padding * 2, height + m_padding * 2);

	if (node)
		return { node->x + m_padding, node->y + m_padding, width, height };
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
	assert(x < m_width);
	assert((x + width) <= m_width);
	assert(y < m_height);
	assert((y + height) <= m_height);
	assert(stride == m_numComponents);
	unsigned char red[] = { 255, 0, 0, 255 };

	if (width == m_width && height == m_height)
	{
		memcpy(m_data, data, width * height * m_numComponents);
	}
	else
	{
		unsigned char pixel[] = { 0, 0, 0, 0 };
		// NOTE: can optimize a whole lot
		// center
		for (int xPix = 0; xPix < width; ++xPix)
		{
			for (int yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, xPix, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix, y + yPix, m_numComponents, pixel);
			}
		}
		// top
		
		for (int xPix = 0; xPix < width; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				getPixel(data, width, height, xPix, 0, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix, y - yPix - 1, m_numComponents, pixel);
			}
		}
		//bottom
		for (int xPix = 0; xPix < width; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				getPixel(data, width, height, xPix, height - 1, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix, y + yPix + height, m_numComponents, pixel);
			}
		}
		//left
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, 0, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x - xPix - 1, y + yPix, m_numComponents, pixel);
			}
		}
		//right
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, width - 1, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix + width, y + yPix, m_numComponents, pixel);
			}
		}
		// topLeft
		getPixel(data, width, height, 0, 0, m_numComponents, pixel);
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x - xPix - 1, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// topRight
		getPixel(data, width, height, width - 1, 0, m_numComponents, pixel);
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix + width, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// bottomLeft
		getPixel(data, width, height, 0, height - 1, m_numComponents, pixel);
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x - xPix - 1, y + yPix + height, m_numComponents, pixel);
			}
		}
		// bottomRight
		getPixel(data, width, height, width - 1, height - 1, m_numComponents, pixel);
		for (int xPix = 0; xPix < m_padding; ++xPix)
		{
			for (int yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix + width, y + yPix + height, m_numComponents, pixel);
			}
		}
		
	}
}