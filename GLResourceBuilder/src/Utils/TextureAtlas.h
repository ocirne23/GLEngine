#pragma once

class TextureAtlas
{
public:
	struct AtlasRegion
	{
		int x, y, width, height;
	};

	TextureAtlas(int width, int height, int numComponents);
	~TextureAtlas();

	AtlasRegion getRegion(int width, int height);
	void setRegion(int x, int y, int width, int height, const unsigned char* data, int stride);

public:

	int m_width;
	int m_height;
	int m_numComponents;
	unsigned char* m_data;

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

	Node* getRegion(Node *node, int width, int height);
private:

	Node m_root;
};