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
	if (a_assimpMaterial.GetTextureCount(aiTextureType_SHININESS))
	{
		a_assimpMaterial.GetTexture(aiTextureType_SHININESS, 0, &path);
		m_atlasRegions[ETexTypes_Roughness].m_filePath = path.C_Str();
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

uint64 DBMaterial::getByteSize() const
{
	uint64 totalSize = 0;

	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);

	for (const DBAtlasRegion& r : m_atlasRegions)
		totalSize += r.getByteSize();

	totalSize += AssetDatabaseEntry::getValWriteSize(m_materialColor);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_specColor);

	totalSize += AssetDatabaseEntry::getValWriteSize(m_smoothness);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_metalness);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_opacity);
	return totalSize;
}

void DBMaterial::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);

	for (DBAtlasRegion& r : m_atlasRegions)
		r.write(entry);

	entry.writeVal(m_materialColor);
	entry.writeVal(m_specColor);

	entry.writeVal(m_smoothness);
	entry.writeVal(m_metalness);
	entry.writeVal(m_opacity);
}

void DBMaterial::read(AssetDatabaseEntry& entry)
{
	entry.readString(m_name);

	for (DBAtlasRegion& r : m_atlasRegions)
		r.read(entry);

	entry.readVal(m_materialColor);
	entry.readVal(m_specColor);

	entry.readVal(m_smoothness);
	entry.readVal(m_metalness);
	entry.readVal(m_opacity);
}