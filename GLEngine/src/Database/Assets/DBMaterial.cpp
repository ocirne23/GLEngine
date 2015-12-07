#include "Database/Assets/DBMaterial.h"

#include <assimp/scene.h>

DBMaterial::DBMaterial(const aiMaterial& a_assimpMaterial)
{
	aiString path;
	if (a_assimpMaterial.GetTextureCount(aiTextureType_DIFFUSE))
	{
		a_assimpMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &path);
		setDiffuseTexturePath(path.C_Str());
	}

	if (a_assimpMaterial.GetTextureCount(aiTextureType_NORMALS))
	{
		a_assimpMaterial.GetTexture(aiTextureType_NORMALS, 0, &path);
		setNormalTexturePath(path.C_Str());
	}
	else if (a_assimpMaterial.GetTextureCount(aiTextureType_HEIGHT)) //.obj uses height type for normals
	{
		a_assimpMaterial.GetTexture(aiTextureType_HEIGHT, 0, &path);
		setNormalTexturePath(path.C_Str());
	}
	a_assimpMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, (aiColor4D&) m_materialColor);
	a_assimpMaterial.Get(AI_MATKEY_OPACITY, m_opacity);
	a_assimpMaterial.Get(AI_MATKEY_SHININESS, m_shininess);
	a_assimpMaterial.Get(AI_MATKEY_COLOR_SPECULAR, (aiColor4D&) m_specColor);
}

const eastl::string& DBMaterial::getDiffuseTexturePath() const
{
	return m_diffuseRegion.m_filePath;
}

const eastl::string& DBMaterial::getNormalTexturePath() const
{
	return m_normalRegion.m_filePath;
}

void DBMaterial::setDiffuseAtlasRegion(const DBAtlasRegion& a_region)
{
	m_diffuseRegion = a_region;
}

void DBMaterial::setNormalAtlasRegion(const DBAtlasRegion& a_region)
{
	m_normalRegion = a_region;
}

void DBMaterial::setDiffuseTexturePath(const eastl::string& a_filePath)
{
	m_diffuseRegion.m_filePath = a_filePath;
}

void DBMaterial::setNormalTexturePath(const eastl::string& a_filePath)
{
	m_normalRegion.m_filePath = a_filePath;
}

uint64 DBMaterial::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += m_diffuseRegion.getByteSize();
	totalSize += m_normalRegion.getByteSize();
	totalSize += sizeof(m_materialColor);
	totalSize += sizeof(m_roughness);
	totalSize += sizeof(m_metalness);

	totalSize += sizeof(m_opacity);
	totalSize += sizeof(m_shininess);
	totalSize += sizeof(m_specColor);
	return totalSize;
}

void DBMaterial::write(AssetDatabaseEntry& entry)
{
	m_diffuseRegion.write(entry);
	m_normalRegion.write(entry);
	entry.writeVal(m_materialColor);
	entry.writeVal(m_roughness);
	entry.writeVal(m_metalness);

	entry.writeVal(m_opacity);
	entry.writeVal(m_shininess);
	entry.writeVal(m_specColor);
}

void DBMaterial::read(AssetDatabaseEntry& entry)
{
	m_diffuseRegion.read(entry);
	m_normalRegion.read(entry);
	entry.readVal(m_materialColor);
	entry.readVal(m_roughness);
	entry.readVal(m_metalness);

	entry.readVal(m_opacity);
	entry.readVal(m_shininess);
	entry.readVal(m_specColor);
}