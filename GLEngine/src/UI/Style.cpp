#include "UI/Style.h"

#include "Graphics/GL/Wrappers/GLTexture.h"
#include "json/json.h"
#include "Utils/FileHandle.h"
#include <assert.h>

Style::~Style()
{
	if (!m_isDisposed)
	{
		for (auto it : m_textures)
		{
			delete it.second;
		}
	}
}

void Style::load(const char* a_filePath)
{
	Json::Value json;
	Json::Reader reader;
	{
		FileHandle handle(a_filePath);
		eastl::string contents = handle.readString();
		bool success = reader.parse(contents.begin(), contents.end(), json);
		assert(success);
	}
	
	m_textButtonTex.texture  = getTexture(json["textbutton"].asCString());
	m_imageButtonTex.texture = getTexture(json["imagebutton"].asCString());
}

GLTexture* Style::getTexture(const char* a_filePath)
{
	GLTexture* texture = NULL;
	auto it = m_textures.find(eastl::string(a_filePath));
	if (it == m_textures.end())
	{
		texture = new GLTexture();
		texture->initialize(a_filePath, GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::NEAREST);
		m_textures.insert({eastl::string(a_filePath), texture});
	}
	else
	{
		texture = it->second;
	}
	return texture;
}