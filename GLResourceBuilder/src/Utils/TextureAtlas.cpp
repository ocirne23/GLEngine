#include "TextureAtlas.h"

#include <assert.h>
#include <memory>

BEGIN_UNNAMED_NAMESPACE()

inline void setPixel(byte* a_data, uint a_width, uint a_height, uint a_x, uint a_y, uint a_numComponents, const byte* a_pixelData)
{
	memcpy(a_data + ((a_width * a_y) + a_x) * a_numComponents, a_pixelData, a_numComponents);
}

inline void getPixel(const byte* a_data, uint a_width, uint a_height, uint a_x, uint a_y, uint a_numComponents, byte* a_outPixelData)
{
	memcpy(a_outPixelData, a_data + ((a_width * a_y) + a_x) * a_numComponents, a_numComponents);
}

END_UNNAMED_NAMESPACE()

TextureAtlas::TextureAtlas(uint a_width, uint a_height, uint a_numComponents, uint a_numMipMaps)
{
	initialize(a_width, a_height, a_numComponents, a_numMipMaps);
}

TextureAtlas::~TextureAtlas()
{
	delete [] m_data;
}

TextureAtlas::Node::~Node()
{
	if (left)
		delete left;
	if (right)
		delete right;
}

TextureAtlas::AtlasRegion TextureAtlas::getRegion(uint width, uint height)
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
		return 0;
	}

	if (width > node->width || height > node->height)
		return 0;

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

void TextureAtlas::setRegion(uint x, uint y, uint width, uint height, const unsigned char* data, uint stride)
{
	assert(x < m_width);
	assert((x + width) <= m_width);
	assert(y < m_height);
	assert((y + height) <= m_height);
	assert(stride == m_numComponents);

	if (m_data == NULL)
	{
		m_data = new unsigned char[m_width * m_height * m_numComponents];

		unsigned char red [] = {255, 0, 0, 255};
		// Initialize atlas color to red
		for (uint xa = 0; xa < m_width; ++xa)
			for (uint ya = 0; ya < m_height; ++ya)
				setPixel(m_data, m_width, m_height, xa, ya, m_numComponents, red);
	}

	if (width == m_width && height == m_height)
	{
		memcpy(m_data, data, width * height * m_numComponents);
	}
	else
	{	// NOTE: can optimize a whole lot
		unsigned char pixel [] = {0, 0, 0, 0};
		// center
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, xPix, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix, y + yPix, m_numComponents, pixel);
			}
		}
		// top
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			getPixel(data, width, height, xPix, 0, m_numComponents, pixel);
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix, y - yPix - 1, m_numComponents, pixel);
			}
		}
		//bottom
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			getPixel(data, width, height, xPix, height - 1, m_numComponents, pixel);
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix, y + yPix + height, m_numComponents, pixel);
			}
		}
		//left
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, 0, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x - xPix - 1, y + yPix, m_numComponents, pixel);
			}
		}
		//right
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(data, width, height, width - 1, yPix, m_numComponents, pixel);
				setPixel(m_data, m_width, m_height, x + xPix + width, y + yPix, m_numComponents, pixel);
			}
		}
		// topLeft
		getPixel(data, width, height, 0, 0, m_numComponents, pixel);
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x - xPix - 1, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// topRight
		getPixel(data, width, height, width - 1, 0, m_numComponents, pixel);
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix + width, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// bottomLeft
		getPixel(data, width, height, 0, height - 1, m_numComponents, pixel);
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x - xPix - 1, y + yPix + height, m_numComponents, pixel);
			}
		}
		// bottomRight
		getPixel(data, width, height, width - 1, height - 1, m_numComponents, pixel);
		for (uint xPix = 0; xPix < m_padding; ++xPix)
		{
			for (uint yPix = 0; yPix < m_padding; ++yPix)
			{
				setPixel(m_data, m_width, m_height, x + xPix + width, y + yPix + height, m_numComponents, pixel);
			}
		}
	}
}

void TextureAtlas::clear()
{
	initialize(m_width, m_height, m_numComponents, m_numMipMaps);
}

void TextureAtlas::initialize(uint a_width, uint a_height, uint a_numComponents, uint a_numMipMaps)
{
	assert(a_width % 2 == 0 && a_height % 2 == 0 && "Atlas width and height must be divideable by 2");

	if (m_width != a_width || m_height != a_height || m_numComponents != a_numComponents)
	{
		// Lazy initialisation for m_data inside setRegion since algorithms may create atlases to see if textures fit
		// with the current size and resize the atlases if they don't.
		if (m_data)
		{
			delete [] m_data;
			m_data = NULL;
		}

		m_width = a_width;
		m_height = a_height;
		m_numComponents = a_numComponents;
		m_padding = a_numMipMaps * a_numMipMaps;
	}

	if (m_root.left)
		delete m_root.left;
	if (m_root.right)
		delete m_root.right;
	m_root.left = m_root.right = NULL;

	m_root.x = m_root.y = 0;
	m_root.width = a_width;
	m_root.height = a_height;
}