#pragma once

class TextureAtlas
{
public:

	struct AtlasRegion
	{
		uint x, y, width, height;
	};

public:

	TextureAtlas(uint width, uint height, uint numComponents);
	~TextureAtlas();

	void clear();
	void initialize(uint width, uint height, uint numComponents);

	AtlasRegion getRegion(uint width, uint height);
	void setRegion(uint x, uint y, uint width, uint height, const unsigned char* data, uint stride);

public:

	uint m_width          = 0;
	uint m_height         = 0;
	uint m_numComponents  = 0;
	uint m_padding        = 0;
	unsigned char* m_data = 0;

private:

	struct Node
	{
		~Node();

		Node* left  = 0;
		Node* right = 0;
		uint x      = 0;
		uint y      = 0;
		uint width  = 0;
		uint height = 0;
	};

	const Node* getRegion(Node *node, uint width, uint height);

private:

	Node m_root;
};