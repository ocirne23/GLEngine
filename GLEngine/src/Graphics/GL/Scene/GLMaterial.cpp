#include "Graphics/GL/Scene/GLMaterial.h"

#include "Database/Assets/DBMaterial.h"

void GLMaterial::initialize(const DBMaterial& a_material)
{
	diffuseMapping = a_material.getDiffuseRegion().m_atlasMapping;
	normalMapping = a_material.getNormalRegion().m_atlasMapping;
	diffuseAtlasIdx = a_material.getDiffuseRegion().m_atlasIdx;
	normalAtlasIdx = a_material.getNormalRegion().m_atlasIdx;
	roughness = a_material.getRoughness();
	metalness = a_material.getMetalness();
}
