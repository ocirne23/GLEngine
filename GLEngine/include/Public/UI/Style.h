#pragma once

#include "UI/Font.h"

#include "UI/TextureRegion.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"

class GLTexture;

class Style
{
public:

	~Style();
	void load(const char* filePath);
	void dispose();

private:

	GLTexture* getTexture(const rde::string& filePath);

public:

	TextureRegion m_textButtonStyle;
	TextureRegion m_imageButtonStyle;

private:

	bool m_isDisposed = false;
	rde::hash_map<const char*, GLTexture*> m_textures;
};