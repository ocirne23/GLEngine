#pragma once

#include "Core.h"

#include "rde/hash_map.h"
#include "rde/rde_string.h"

class GLTexture;

class GLTextureManager
{
public:
	friend class Graphics;

	union TextureHandle
	{
		struct Bits
		{
			uint lower;
			uint upper;
		};
		Bits bits;
		uint64 handle;
	};

public:

	TextureHandle getTextureHandle(const rde::string& filePath);

private:
	GLTextureManager();
	~GLTextureManager();

private:

	rde::hash_map<rde::string, rde::pair<GLTexture*, TextureHandle>> m_handleMap;
};