#pragma once
/*
#include "Core.h"

#include <rde\hash_map.h>
#include <rde\vector.h>
#include <rde\rde_string.h>

class Pixmap;
class GLTextureArray;

class GLTextureManager
{
public:

	struct TextureHandle
	{
		uint textureArrayNr;
		uint arrayIndex;

		TextureHandle() : textureArrayNr(0xFFFFFFFF), arrayIndex(0xFFFFFFFF) {}
		TextureHandle(uint textureArrayNr, uint arrayIndex) : textureArrayNr(textureArrayNr), arrayIndex(arrayIndex) {}
	};
	class TextureBinder
	{
	public:
		friend class GLTextureManager;

		const TextureHandle createTextureHandle(const char* filename);
		void bindTextureArrays(int uniformLoc, uint fromTextureUnit, uint maxTextureUnit);
		uint getNumTextureArrays();

	private:
		TextureBinder(GLTextureManager& textureManager) : m_textureManager(textureManager) {};
		~TextureBinder() {};
		uint addTextureArray(GLTextureArray* textureArray);

	private:

		GLTextureManager& m_textureManager;
		rde::vector<GLTextureArray*> m_textureArrays;
		rde::vector<int> m_textureBinds;
	};

public:

	GLTextureManager() {};
	~GLTextureManager();

	TextureBinder* createTextureBinder();
	void initializeTextureBinders();

private:

	struct TextureArrayConfig
	{
		GLTextureArray* m_textureArray;
		rde::vector<Pixmap*> m_pixmaps;
	};
	struct ManagedTextureHandle
	{
		GLTextureArray* textureArray;
		uint arrayIndex;
	};

private:

	const TextureHandle createTextureHandle(GLTextureManager::TextureBinder& textureBinder, const char* filename);

private:

	rde::vector<TextureBinder*> m_textureBinders;
	rde::vector<TextureArrayConfig*> m_textureArrayConfigs;
	rde::hash_map<rde::string, ManagedTextureHandle> m_managedTextures;
};
*/

