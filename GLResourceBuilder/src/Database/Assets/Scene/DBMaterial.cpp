#include "Database/Assets/Scene/DBMaterial.h"

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
}

const std::string& DBMaterial::getDiffuseTexturePath() const
{
	return m_diffuseRegion.m_filePath;
}

const std::string& DBMaterial::getNormalTexturePath() const
{
	return m_normalRegion.m_filePath;
}

void DBMaterial::setDiffuseTextureRegion(const DBAtlasRegion& a_region)
{
	m_diffuseRegion = a_region;
}

void DBMaterial::setNormalTextureRegion(const DBAtlasRegion& a_region)
{
	m_normalRegion = a_region;
}

void DBMaterial::setDiffuseTexturePath(const std::string& a_filePath)
{
	m_diffuseRegion.m_filePath = a_filePath;
}

void DBMaterial::setNormalTexturePath(const std::string& a_filePath)
{
	m_normalRegion.m_filePath = a_filePath;
}

uint64 DBMaterial::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += m_diffuseRegion.getByteSize();
	totalSize += m_normalRegion.getByteSize();
	totalSize += sizeof(m_vertexColor);
	totalSize += sizeof(m_roughness);
	totalSize += sizeof(m_metalness);
	return totalSize;
}

void DBMaterial::write(AssetDatabaseEntry& entry)
{
	m_diffuseRegion.write(entry);
	m_normalRegion.write(entry);
	entry.writeVal(m_vertexColor);
	entry.writeVal(m_roughness);
	entry.writeVal(m_metalness);
}

void DBMaterial::read(AssetDatabaseEntry& entry)
{

}