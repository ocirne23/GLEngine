#pragma once

#include "BuilderCore.h"

class TextureAtlas
{
public:

	struct AtlasRegion
	{
		AtlasRegion() {}
		AtlasRegion(uint x, uint y, uint width, uint height) : x(x), y(y), width(width), height(height) {}
		uint x      = 0; 
		uint y      = 0; 
		uint width  = 0;
		uint height = 0;
	};

public:

	TextureAtlas(uint width, uint height, uint numComponents, uint numMipMaps);
	~TextureAtlas();

	void clear();
	void initialize(uint width, uint height, uint numComponents, uint numMipMaps);

	AtlasRegion getRegion(uint width, uint height);
	void setRegion(uint x, uint y, uint width, uint height, const unsigned char* data, uint stride);

public:

	uint m_width         = 0;
	uint m_height        = 0;
	uint m_numComponents = 0;
	uint m_padding       = 0;
	uint m_numMipMaps    = 0;
	byte* m_data         = 0;

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