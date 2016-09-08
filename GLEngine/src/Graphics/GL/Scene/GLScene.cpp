#include "Graphics/GL/Scene/GLScene.h"

#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"
#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Scene/GLRenderer.h"

void GLScene::initialize(const eastl::string& a_assetName, AssetDatabase& a_database)
{
	DBScene* scene = dcast<DBScene*>(a_database.loadAsset(a_assetName, EAssetType::SCENE));
	scene->mergeMeshes();
	initialize(*scene);
	a_database.unloadAsset(scene);
}

void GLScene::initialize(const DBScene& a_dbScene)
{
	//print("numMeshes: %i\nnumMaterials: %i\nnumNodes: %i\n", scene->numMeshes(), scene->numMaterials(), scene->numNodes());
	m_nodes = a_dbScene.getNodes();
	m_materials = a_dbScene.getMaterials();
	m_meshes.resize(a_dbScene.numMeshes());
	for (uint i = 0; i < a_dbScene.numMeshes(); ++i)
		m_meshes[i].initialize(a_dbScene.getMeshes()[i]);
	m_materialBuffer.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::MaterialProperties));
	updateMaterialBuffer();

	for (uint i = 0; i < DBMaterial::ETexTypes_COUNT; ++i)
	{
		const eastl::vector<DBAtlasTexture>& atlasTextures = a_dbScene.getAtlasTextures(DBMaterial::ETexTypes(i));
		if (atlasTextures.empty())
			continue;

		// Use info from the first texture since all textures use the same format.
		const DBTexture& tex = atlasTextures[0].getTexture();
		m_textureArrays[i].startInit(tex.getWidth(), tex.getHeight(), uint(atlasTextures.size()), tex.getNumComponents(),
			(tex.getFormat() == DBTexture::EFormat::FLOAT), atlasTextures[0].getNumMipmaps());

		for (const DBAtlasTexture& atlasTexture : atlasTextures)
			m_textureArrays[i].addTexture(atlasTexture.getTexture());
		m_textureArrays[i].finishInit();
	}
}

void GLScene::render(GLRenderer& a_renderer, const glm::mat4& a_transform, bool a_depthOnly)
{
	if (!a_depthOnly)
	{
		m_materialBuffer.bind();
		for (uint i = 0; i < DBMaterial::ETexTypes_COUNT; ++i)
		{
			if (m_textureArrays[i].getNumComponents())
				m_textureArrays[i].bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures(uint(GLConfig::ETextures::DiffuseAtlasArray) + i)));
			else
				m_textureArrays[i].unbind(GLConfig::getTextureBindingPoint(GLConfig::ETextures(uint(GLConfig::ETextures::DiffuseAtlasArray) + i)));
		}
	}
	renderNode(m_nodes[0], a_renderer, a_transform);
}

void GLScene::renderNode(const DBNode& a_node, GLRenderer& a_renderer, const glm::mat4& a_parentTransform)
{
	GLRenderer::ModelData data;
	data.u_modelMatrix = a_parentTransform * a_node.getTransform();
	data.u_normalMatrix = glm::inverse(glm::transpose(data.u_modelMatrix * a_renderer.getSceneCamera()->getViewMatrix()));
	a_renderer.setModelDataUBO(data);

	for (uint i : a_node.getMeshIndices())
		m_meshes[i].render();
	for (uint i : a_node.getChildIndices())
		renderNode(m_nodes[i], a_renderer, data.u_modelMatrix);
}

void GLScene::updateMaterialBuffer()
{
	uint numMaterials = uint(m_materials.size());
	if (numMaterials > GLConfig::getMaxMaterials())
	{
		print("Scene has more than %i materials\n", GLConfig::getMaxMaterials());
		numMaterials = GLConfig::getMaxMaterials();
	}
	GLMaterial* materialBuffer = rcast<GLMaterial*>(m_materialBuffer.mapBuffer());
	for (uint i = 0; i < numMaterials; ++i)
	{
		materialBuffer->initialize(m_materials[i]);
		materialBuffer++;
	}
	m_materialBuffer.unmapBuffer();
}

void GLScene::updateMaterialBuffer(const DBMaterial& material, uint idx)
{
	GLMaterial mat;
	mat.initialize(material);
	m_materialBuffer.upload(sizeof(mat), &mat, sizeof(mat) * idx);
}

/*
glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(m_scale));
glm::mat4 rotation = glm::rotate(glm::mat4(1), m_axisRotation.w, glm::vec3(m_axisRotation));
glm::mat4 translation = glm::translate(glm::mat4(1), m_translation);
m_baseTransform = translation * rotation * scale;
*/