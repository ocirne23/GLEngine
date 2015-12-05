#pragma once

#include "Graphics/Utils/TextureRegion.h"
#include "Graphics/UI/Font.h"
#include "EASTL/string.h"
#include "EASTL/hash_map.h"

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
	eastl::hash_map<eastl::string, GLTexture*> m_textures;
};