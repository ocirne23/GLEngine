#include "TextureAtlas.h"

void RectangleBinPack::init(int width, int height)
{
	binWidth = width;
	binHeight = height;
	root.left = root.right = 0;
	root.x = root.y = 0;
	root.width = width;
	root.height = height;
}

float RectangleBinPack::occupancy() const
{
	unsigned long totalSurfaceArea = binWidth * binHeight;
	unsigned long area = usedSurfaceArea(root);

	return (float) area / totalSurfaceArea;
}

unsigned long RectangleBinPack::usedSurfaceArea(const Node &node) const
{
	if (node.left || node.right)
	{
		unsigned long area = node.width * node.height;
		if (node.left)
			area += usedSurfaceArea(*node.left);
		if (node.right)
			area += usedSurfaceArea(*node.right);

		return area;
	}

	return 0;
}

RectangleBinPack::AtlasRegion RectangleBinPack::insert(RectangleBinPack::Node *node, int width, int height)
{
	if (node->left || node->right)
	{
		if (node->left)
		{
			AtlasRegion region = insert(node->left, width, height);
			if (region.width && region.height)
				return region;
		}
		if (node->right)
		{
			AtlasRegion region = insert(node->right, width, height);
			if (region.width && region.height)
				return region;
		}
		return{ 0, 0, 0, 0 };
	}

	if (width > node->width || height > node->height)
		return{ 0, 0, 0, 0 };

	int w = node->width - width;
	int h = node->height - height;
	node->left = new Node;
	node->right = new Node;
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
	return{ node->x, node->y, node->width, node->height };
}