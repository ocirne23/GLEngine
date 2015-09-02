#pragma once

#include "UI/Font.h"

#include "Graphics/TextureRegion.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"

class GLTexture;

class Style
{
public:

	~Style();
	void load(const char* filePath);
	void dispose();

	const TextureRegion& getTextButtonTex() const { return m_textButtonTex; }
	const TextureRegion& getImageButtonTex() const { return m_imageButtonTex; }

private:

	GLTexture* getTexture(const char* filePath);

private:

	TextureRegion m_textButtonTex;
	TextureRegion m_imageButtonTex;

	bool m_isDisposed = false;
	rde::hash_map<rde::string, GLTexture*> m_textures;
};