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
	a_assimpMaterial.Get(AI_MATKEY_SHININESS, m_smoothness);
	m_smoothness /= 4.0f;
	a_assimpMaterial.Get(AI_MATKEY_REFRACTI, m_metalness);
	aiString name;
	a_assimpMaterial.Get(AI_MATKEY_NAME, name);
	m_name = name.C_Str();
}

const eastl::string& DBMaterial::getDiffuseTexturePath() const
{
	return m_diffuseRegion.m_filePath;
}

const eastl::string& DBMaterial::getNormalTexturePath() const
{
	return m_normalRegion.m_filePath;
}

void DBMaterial::setDiffuseRegion(const DBAtlasRegion& a_region)
{
	m_diffuseRegion = a_region;
}

void DBMaterial::setNormalRegion(const DBAtlasRegion& a_region)
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
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);
	totalSize += m_diffuseRegion.getByteSize();
	totalSize += m_normalRegion.getByteSize();
	totalSize += sizeof(m_materialColor);
	totalSize += sizeof(m_specColor);

	totalSize += sizeof(m_smoothness);
	totalSize += sizeof(m_metalness);
	totalSize += sizeof(m_opacity);
	return totalSize;
}

void DBMaterial::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);
	m_diffuseRegion.write(entry);
	m_normalRegion.write(entry);
	entry.writeVal(m_materialColor);
	entry.writeVal(m_specColor);

	entry.writeVal(m_smoothness);
	entry.writeVal(m_metalness);
	entry.writeVal(m_opacity);
}

void DBMaterial::read(AssetDatabaseEntry& entry)
{
	entry.readString(m_name);
	m_diffuseRegion.read(entry);
	m_normalRegion.read(entry);
	entry.readVal(m_materialColor);
	entry.readVal(m_specColor);

	entry.readVal(m_smoothness);
	entry.readVal(m_metalness);
	entry.readVal(m_opacity);
}