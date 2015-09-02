#include "UI/Style.h"

#include "Graphics/GL/Wrappers/GLTexture.h"
#include "3rdparty/json/json.h"
#include "Utils/FileHandle.h"

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
		rde::string contents = handle.readString();
		bool success = reader.parse(contents.begin(), contents.end(), json);
		assert(success);
	}
	
	m_textButtonTex.texture  = getTexture(rde::string(json["textbutton"]));
	m_imageButtonTex.texture = getTexture(rde::string(json["imagebutton"]));
}

GLTexture* Style::getTexture(const rde::string& a_filePath)
{
	GLTexture* texture = NULL;
	auto it = m_textures.find(a_filePath.c_str());
	if (it == m_textures.end())
	{
		texture = new GLTexture();
		texture->initialize(a_filePath.c_str());
		assert(texture->isLoaded());
	}
	else
	{
		texture = it->second;
	}
	return texture;
}