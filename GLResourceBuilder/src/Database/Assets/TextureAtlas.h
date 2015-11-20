#pragma once

#include "BuilderCore.h"
#include "Utils/Vec.h"

class TextureAtlas
{
public:

	TextureAtlas(uint width, uint height, uint numComponents, uint numMipMaps, uint atlasIndex);
	~TextureAtlas() { SAFE_DELETE_ARRAY(m_data); }

	void clear();
	void initialize(uint width, uint height, uint numComponents, uint numMipMaps, uint atlasIndex);

	Vec4 getRegion(uint width, uint height);
	void setRegion(uint x, uint y, uint width, uint height, const unsigned char* data, uint stride);

public:

	uint m_width         = 0;
	uint m_height        = 0;
	uint m_numComponents = 0;
	uint m_padding       = 0;
	uint m_numMipMaps    = 0;
	byte* m_data         = 0;
	uint m_atlasIndex    = 0;

private:

	struct Node
	{
		~Node() { SAFE_DELETE(left); SAFE_DELETE(right); }
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