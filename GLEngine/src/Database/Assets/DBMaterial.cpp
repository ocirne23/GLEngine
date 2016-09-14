#include "Database/Assets/DBMaterial.h"

#include <assimp/scene.h>

DBMaterial::DBMaterial(const aiMaterial& a_assimpMaterial)
{
	aiString path;
	if (a_assimpMaterial.GetTextureCount(aiTextureType_DIFFUSE))
	{
		a_assimpMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &path);
		m_atlasRegions[ETexTypes_Diffuse].m_filePath = path.C_Str();
	}
	if (a_assimpMaterial.GetTextureCount(aiTextureType_NORMALS))
	{
		a_assimpMaterial.GetTexture(aiTextureType_NORMALS, 0, &path);
		m_atlasRegions[ETexTypes_Normal].m_filePath = path.C_Str();
	}
	else if (a_assimpMaterial.GetTextureCount(aiTextureType_HEIGHT)) //.obj uses height type for normals
	{
		a_assimpMaterial.GetTexture(aiTextureType_HEIGHT, 0, &path);
		m_atlasRegions[ETexTypes_Normal].m_filePath = path.C_Str();
	}
	if (a_assimpMaterial.GetTextureCount(aiTextureType_AMBIENT))
	{
		a_assimpMaterial.GetTexture(aiTextureType_AMBIENT, 0, &path);
		m_atlasRegions[ETexTypes_Metalness].m_filePath = path.C_Str();
	}
	if (a_assimpMaterial.GetTextureCount(aiTextureType_OPACITY))
	{
		a_assimpMaterial.GetTexture(aiTextureType_OPACITY, 0, &path);
		m_atlasRegions[ETexTypes_Opacity].m_filePath = path.C_Str();
	}
	if (a_assimpMaterial.GetTextureCount(aiTextureType_SPECULAR))
	{
		a_assimpMaterial.GetTexture(aiTextureType_SPECULAR, 0, &path);
		m_atlasRegions[ETexTypes_Roughness].m_filePath = path.C_Str();
	}

	aiString name;
	a_assimpMaterial.Get(AI_MATKEY_NAME, name);
	m_name = name.C_Str();
}

uint64 DBMaterial::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);
	for (const DBAtlasRegion& r : m_atlasRegions)
		totalSize += r.getByteSize();
	return totalSize;
}

void DBMaterial::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);
	for (DBAtlasRegion& r : m_atlasRegions)
		r.write(entry);
}

void DBMaterial::read(AssetDatabaseEntry& entry)
{
	entry.readString(m_name);
	for (DBAtlasRegion& r : m_atlasRegions)
		r.read(entry);
}