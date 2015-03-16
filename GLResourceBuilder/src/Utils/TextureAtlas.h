#pragma once

class TextureAtlas
{
public:

    struct AtlasRegion
    {
	int x, y, width, height;
    };

public:

    TextureAtlas(int width, int height, int numComponents, int numMipMaps);
    ~TextureAtlas();

    void clear();
    void initialize(int width, int height, int numComponents, int numMipMaps);

    AtlasRegion getRegion(int width, int height);
    void setRegion(int x, int y, int width, int height, const unsigned char* data, int stride);

public:

    int m_width = 0;
    int m_height = 0;
    int m_numComponents = 0;
    int m_numMipMaps = 0;
    int m_padding = 0;
    unsigned char* m_data = 0;

private:

    struct Node
    {
	~Node();

	Node* left = 0;
	Node* right = 0;

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
    };

    const Node* getRegion(Node *node, int width, int height);

private:

    Node m_root;
};