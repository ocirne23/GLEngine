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
	for (TextureArrayData* textureArrayData : m_textureArrayData)
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
	for (TextureArrayData* textureArrayData : m_textureArrayData)
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
	GLTextureManager::TextureHandle handle = m_textureManager.createTextureHandle(*this, filename);
	print("handle %i %i \n", handle.arrayIndex, handle.textureArrayNr);
	return handle;
}

const GLTextureManager::TextureHandle GLTextureManager::createTextureHandle(GLTextureManager::TextureBinder& textureBinder, const char* filename)
{
	// If file was already added
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
		pixmap->readRaw(filename);
		assert(pixmap->exists());

		for (int i = 0; i < m_textureArrayData.size(); ++i)
		{
			TextureArrayData* textureArrayData = m_textureArrayData[i];

			if (textureArrayData->m_textureArray->getWidth() == pixmap->m_width &&
				textureArrayData->m_textureArray->getHeight() == pixmap->m_height &&
				textureArrayData->m_textureArray->getNumComponents() == pixmap->m_numComponents)
			{
				uint pixmapIndex = textureArrayData->m_pixmaps.size();
				textureArrayData->m_pixmaps.push_back(pixmap);
				return { i, pixmapIndex };
			}
		}

		TextureArrayData* data = new TextureArrayData();
		m_textureArrayData.push_back(data);
		data->m_textureArray = new GLTextureArray();
		data->m_textureArray->setDimensions(pixmap->m_width, pixmap->m_height, pixmap->m_numComponents);
		unsigned int arrayNr = textureBinder.addTextureArray(data->m_textureArray);
		data->m_pixmaps.push_back(pixmap);

		return{ m_textureArrayData.size() - 1, arrayNr };
	}
}