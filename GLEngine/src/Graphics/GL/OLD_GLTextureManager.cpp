/*
#include "Graphics\GL\GLTextureManager.h"

#include "GLEngine.h"
#include "Graphics\Graphics.h"

#include "Graphics\Pixmap.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLTextureArray.h"
#include "Graphics\GL\Core\GLShader.h"

inline uint min(uint a, uint b) { return a < b ? a : b; }

GLTextureManager::~GLTextureManager()
{
	for (TextureArrayConfig* textureArrayData : m_textureArrayConfigs)
	{
		for (const Pixmap* pixmap : textureArrayData->m_pixmaps)
		{
			delete pixmap;
		}
		delete textureArrayData;
	}

	for (TextureBinder* textureBinder : m_textureBinders)
	{
		delete textureBinder;
	}
}

GLTextureManager::TextureBinder* GLTextureManager::createTextureBinder()
{
	TextureBinder* binder = new TextureBinder(*this);
	m_textureBinders.push_back(binder);
	return binder;
}

void GLTextureManager::initializeTextureBinders()
{
	for (TextureArrayConfig* textureArrayData : m_textureArrayConfigs)
	{
		textureArrayData->m_textureArray->initialize(textureArrayData->m_pixmaps);
		
		for (const Pixmap* pixmap : textureArrayData->m_pixmaps)
		{
			delete pixmap;
		}
	}
}

void GLTextureManager::TextureBinder::bindTextureArrays(int uniformLoc, uint fromTextureUnit, uint maxTextureUnit)
{
	uint numAvailableTextureUnits = maxTextureUnit - fromTextureUnit;
	assert((uint) m_textureArrays.size() <= numAvailableTextureUnits);

	uint numUsedTextureUnits = m_textureArrays.size();
	m_textureBinds.resize(numUsedTextureUnits);

	for (uint i = 0; i < numUsedTextureUnits; ++i)
	{
		m_textureBinds[i] = i + fromTextureUnit;
		m_textureArrays[i]->bind(i + fromTextureUnit);
	}
	if (m_textureArrays.size() > 0)
	{
		glUniform1iv(uniformLoc, numUsedTextureUnits, &m_textureBinds[0]);
	}
}

uint GLTextureManager::TextureBinder::getNumTextureArrays()
{
	return m_textureArrays.size();
}

uint GLTextureManager::TextureBinder::addTextureArray(GLTextureArray* textureArray)
{
	for (int i = 0; i < m_textureArrays.size(); ++i)
	{
		if (m_textureArrays[i] == textureArray)
			return i;
	}

	m_textureArrays.push_back(textureArray);
	m_textureBinds.push_back();
	return m_textureArrays.size() - 1;
}

const GLTextureManager::TextureHandle GLTextureManager::TextureBinder::createTextureHandle(const char* filename)
{
	return m_textureManager.createTextureHandle(*this, filename);
}

const GLTextureManager::TextureHandle GLTextureManager::createTextureHandle(GLTextureManager::TextureBinder& textureBinder, const char* filename)
{
	auto at = m_managedTextures.find(rde::string(filename));
	if (at != m_managedTextures.end())
	{
		ManagedTextureHandle& handle = at->second;
		uint arrayNr = textureBinder.addTextureArray(handle.textureArray);
		return { arrayNr, handle.arrayIndex };
	}
	else
	{
		Pixmap* pixmap = new Pixmap();
		pixmap->read(filename);
		assert(pixmap->exists());
		
		for (uint i = 0; i < (uint) m_textureArrayConfigs.size(); ++i)
		{
			TextureArrayConfig* textureArrayConfig = m_textureArrayConfigs[i];

			if (textureArrayConfig->m_textureArray->getWidth() == pixmap->m_width &&
				textureArrayConfig->m_textureArray->getHeight() == pixmap->m_height &&
				textureArrayConfig->m_textureArray->getNumComponents() == pixmap->m_numComponents)
			{
				uint pixmapIndex = textureArrayConfig->m_pixmaps.size();
				textureArrayConfig->m_pixmaps.push_back(pixmap);
				return { i, pixmapIndex };
			}
		}

		TextureArrayConfig* config = new TextureArrayConfig();
		m_textureArrayConfigs.push_back(config);
		config->m_textureArray = new GLTextureArray();
		config->m_textureArray->setDimensions(pixmap->m_width, pixmap->m_height, pixmap->m_numComponents, pixmap->m_isFloatData);
		textureBinder.addTextureArray(config->m_textureArray);
		config->m_pixmaps.push_back(pixmap);

		return{ m_textureArrayConfigs.size() - 1u, 0u };
	}
}
*/