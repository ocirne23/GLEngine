#pragma once

#include "Graphics/TextureRegion.h"
#include "UI/Font.h"
#include "eastl/umap.h"

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
	eastl::hash<eastl::string, GLTexture*> m_textures;
};