#include "Graphics/GL/Scene/GLScene.h"

#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"
#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Scene/GLRenderer.h"

void GLScene::initialize(const eastl::string& a_assetName, AssetDatabase& a_database)
{
	DBScene* scene = dcast<DBScene*>(a_database.loadAsset(a_assetName, EAssetType::SCENE));
	initialize(*scene);
	a_database.unloadAsset(scene);
}

void GLScene::initialize(const DBScene& a_dbScene)
{
	m_nodes = a_dbScene.getNodes();
	m_materials = a_dbScene.getMaterials();
	m_meshes.resize(a_dbScene.numMeshes());
	for (uint i = 0; i < a_dbScene.numMeshes(); ++i)
		m_meshes[i].initialize(a_dbScene.getMeshes()[i]);
	m_materialBuffer.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::MaterialProperties));
	updateMaterialBuffer();

	for (DBNode& node : m_nodes)
		node.calculateBounds(a_dbScene.getMeshes());

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
	m_initialized = true;
}

void GLScene::render(GLRenderer& a_renderer, const glm::mat4& a_transform, bool a_depthOnly)
{
	m_materialBuffer.bind();
	// When rendering depth only, we just bind the opacity texture, otherwise every texture.
	for (uint i = (a_depthOnly ? DBMaterial::ETexTypes_Opacity : DBMaterial::ETexTypes_Diffuse); i < DBMaterial::ETexTypes_COUNT; ++i)
	{
		GLTextureArray& atlasArray = m_textureArrays[i];
		if (atlasArray.isInitialized())
			atlasArray.bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures(uint(GLConfig::ETextures::DiffuseAtlasArray) + i)));
		else
			atlasArray.unbind(GLConfig::getTextureBindingPoint(GLConfig::ETextures(uint(GLConfig::ETextures::DiffuseAtlasArray) + i)));
	}
	renderNode(m_nodes[0], a_renderer, a_transform);
}

void GLScene::setAsSkybox(bool a_isSkybox)
{
	m_isSkybox = true;
}

void GLScene::renderNode(const DBNode& a_node, GLRenderer& a_renderer, const glm::mat4& a_parentTransform)
{
	const PerspectiveCamera* camera = a_renderer.getSceneCamera();
	GLRenderer::ModelData data;
	data.u_modelMatrix = a_parentTransform * a_node.getTransform();
	glm::vec3 min = glm::vec3(data.u_modelMatrix * glm::vec4(a_node.getBoundsMin(), 1.0));
	glm::vec3 max = glm::vec3(data.u_modelMatrix * glm::vec4(a_node.getBoundsMax(), 1.0));
	glm::vec3 center = (max + min) / 2.0f;
	glm::vec3 extent = (max - min) / 2.0f;
	if (camera->getFrustum().aabbInFrustum(center, extent) || m_isSkybox)
	{
		data.u_normalMatrix = glm::inverse(glm::transpose(data.u_modelMatrix * camera->getViewMatrix()));
		a_renderer.setModelDataUBO(data);

		for (uint i : a_node.getMeshIndices())
		{
			GLMesh& mesh = m_meshes[i];
			min = glm::vec3(data.u_modelMatrix * glm::vec4(mesh.getBoundsMin(), 1.0));
			max = glm::vec3(data.u_modelMatrix * glm::vec4(mesh.getBoundsMax(), 1.0));
			center = (max + min) / 2.0f;
			extent = (max - min) / 2.0f;
			if (camera->getFrustum().aabbInFrustum(center, extent) || m_isSkybox)
				mesh.render();
		}
		for (uint i : a_node.getChildIndices())
			renderNode(m_nodes[i], a_renderer, data.u_modelMatrix);
	}
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

void GLScene::updateMaterialBuffer(const DBMaterial& a_material, uint a_materialIdx)
{
	GLMaterial mat;
	mat.initialize(a_material);
	m_materialBuffer.upload(sizeof(mat), &mat, sizeof(mat) * a_materialIdx);
}