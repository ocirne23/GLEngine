#include "Graphics/GL/Scene/GLMaterial.h"

#include "Database/Assets/DBMaterial.h"

void GLMaterial::initialize(const DBMaterial& a_material)
{
	diffuseMapping = a_material.getDiffuseRegion().m_atlasMapping;
	normalMapping = a_material.getNormalRegion().m_atlasMapping;
	diffuseAtlasIdx = a_material.getDiffuseRegion().m_atlasIdx;
	normalAtlasIdx = a_material.getNormalRegion().m_atlasIdx;
	materialColor = a_material.getMaterialColor();
	materialColor.a = a_material.getOpacity();

	glm::vec4 specCol = a_material.getSpecularColor();
	roughness = (specCol.r + specCol.g + specCol.b) / 3.0f;
	bool isMetal = glm::abs(materialColor.r - materialColor.g) < 0.05f && glm::abs(materialColor.g - materialColor.b) < 0.05f;
	metalness = isMetal ? 1.0f : 0.0f;

	//roughness = a_material.getRoughness();
	//metalness = a_material.getMetalness();
}
