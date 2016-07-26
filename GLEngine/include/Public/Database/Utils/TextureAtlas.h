#pragma once

#include "Core.h"
#include "gsl/gsl.h"
#include "Database/Utils/AtlasPosition.h"

class TextureAtlas
{
public:

	struct Node
	{
		~Node() { SAFE_DELETE(left); SAFE_DELETE(right); }
		owner<Node*> left = NULL;
		owner<Node*> right = NULL;
		uint x = 0;
		uint y = 0;
		uint width = 0;
		uint height = 0;
	};

public:

	void initialize(uint width, uint height, uint numComponents, uint numMipMaps, uint atlasIndex);
	AtlasPosition getRegion(uint width, uint height);
	void clear();

	uint getWidth() const         { return m_width; }
	uint getHeight() const        { return m_height; }
	uint getNumComponents() const { return m_numComponents; }
	uint getPadding() const       { return m_padding; }
	uint getAtlasIdx() const      { return m_atlasIndex; }
	uint getNumMipmaps() const    { return m_numMipMaps; }

private:

	const Node* getRegion(Node *node, uint width, uint height);

private:

	Node m_root;
	uint m_width         = 0;
	uint m_height        = 0;
	uint m_numMipMaps    = 0;
	uint m_padding       = 0;
	uint m_numComponents = 0;
	uint m_atlasIndex    = 0;
};