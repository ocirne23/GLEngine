#include "UI/Style.h"

#include "Graphics/GL/Wrappers/GLTexture.h"
#include "json/json.h"
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
	
	rde::string imagePath = rde::string(json["textbutton"].asCString());
	GLTexture* textButtonTex = getTexture(imagePath);
	print("loaded tex: %i %i\n", textButtonTex->getWidth(), textButtonTex->getHeight());
}

GLTexture* Style::getTexture(const rde::string& a_filePath)
{
	GLTexture* texture = NULL;
	auto it = m_textures.find(a_filePath.c_str());
	if (it == m_textures.end())
	{
		texture = new GLTexture();
		texture->initialize(FileHandle(a_filePath.c_str()), 0);
		assert(texture->isLoaded());
	}
	else
	{
		texture = it->second;
	}
	return texture;
}