#pragma once

class RectangleBinPack
{
public:
	struct AtlasRegion
	{
		int x, y, width, height;
	};

	void init(int width, int height);

	AtlasRegion insert(int width, int height)
	{
		return insert(&root, width, height);
	}

	/// Computes the ratio of used surface area.
	float occupancy() const;

private:
	struct Node
	{
		Node* left;
		Node* right;

		int x;
		int y;

		int width;
		int height;
	};

	Node root;

	// The total size of the bin we started with.
	int binWidth;
	int binHeight;

	/// @return The surface area used by the subtree rooted at node.
	unsigned long usedSurfaceArea(const Node &node) const;

	/// Inserts a new rectangle in the subtree rooted at the given node.
	AtlasRegion insert(Node *node, int width, int height);
};