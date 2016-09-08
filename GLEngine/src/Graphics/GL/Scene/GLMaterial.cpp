#include "Graphics/GL/Scene/GLMaterial.h"

#include "Database/Assets/DBMaterial.h"

void GLMaterial::initialize(const DBMaterial& a_material)
{
	diffuseMapping   = a_material.getRegion(DBMaterial::ETexTypes_Diffuse).m_atlasMapping;
	normalMapping    = a_material.getRegion(DBMaterial::ETexTypes_Normal).m_atlasMapping;
	metalnessMapping = a_material.getRegion(DBMaterial::ETexTypes_Metalness).m_atlasMapping;
	roughnessMapping = a_material.getRegion(DBMaterial::ETexTypes_Roughness).m_atlasMapping;
	opacityMapping   = a_material.getRegion(DBMaterial::ETexTypes_Opacity).m_atlasMapping;

	diffuseAtlasIdx   = a_material.getRegion(DBMaterial::ETexTypes_Diffuse).m_atlasIdx;
	normalAtlasIdx    = a_material.getRegion(DBMaterial::ETexTypes_Normal).m_atlasIdx;
	metalnessAtlasIdx = a_material.getRegion(DBMaterial::ETexTypes_Metalness).m_atlasIdx;
	roughnessAtlasIdx = a_material.getRegion(DBMaterial::ETexTypes_Roughness).m_atlasIdx;
	opacityAtlasIdx   = a_material.getRegion(DBMaterial::ETexTypes_Opacity).m_atlasIdx;
}
