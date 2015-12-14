#include "Graphics/GL/Scene/GLScene.h"

#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

void GLScene::initialize(const eastl::string& a_assetName, AssetDatabase& a_database)
{
	DBScene* scene = (DBScene*) a_database.loadAsset(a_assetName, EAssetType::SCENE);
	scene->mergeMeshes();
	m_nodes = scene->getNodes();

	//print("numMeshes: %i\nnumMaterials: %i\nnumNodes: %i\n", scene->numMeshes(), scene->numMaterials(), scene->numNodes());

	m_meshes.resize(scene->numMeshes());
	for (uint i = 0; i < scene->numMeshes(); ++i)
		m_meshes[i].initialize(scene->getMeshes()[i]);

	m_materialBuffer.initialize(GLConfig::MATERIAL_PROPERTIES_UBO);

	uint numMaterials = scene->numMaterials();
	if (numMaterials > GLConfig::MAX_MATERIALS)
	{
		print("Scene has more than %i materials\n", GLConfig::MAX_MATERIALS);
		assert(false);
		numMaterials = GLConfig::MAX_MATERIALS;
	}

	GLMaterial* materialBuffer = (GLMaterial*) m_materialBuffer.mapBuffer();
	for (uint i = 0; i < numMaterials; ++i)
	{
		materialBuffer->initialize(scene->getMaterials()[i]);
		materialBuffer++;
	}
	m_materialBuffer.unmapBuffer();

	eastl::vector<GLMaterial> materials;
	materials.resize(scene->numMaterials());
	for (uint i = 0; i < scene->numMaterials(); ++i)
		materials[i].initialize(scene->getMaterials()[i]);

	if (scene->numAtlasTextures())
	{
		const eastl::vector<DBAtlasTexture>& atlasTextures = scene->getAtlasTextures();
		// Use info from the first texture since all textures use the same format.
		const DBAtlasTexture& firstAtlasTex = atlasTextures[0];
		uint width = firstAtlasTex.getTexture().getWidth();
		uint height = firstAtlasTex.getTexture().getHeight();
		uint depth = atlasTextures.size();
		uint numComponents = firstAtlasTex.getTexture().getNumComponents();
		bool isFloatTexture = (firstAtlasTex.getTexture().getFormat() == DBTexture::EFormat::FLOAT);
		uint numMipmaps = firstAtlasTex.getNumMipmaps();

		m_textureArray.startInit(width, height, depth, numComponents, isFloatTexture, numMipmaps);
		for (const DBAtlasTexture& atlasTexture : atlasTextures)
			m_textureArray.addTexture(atlasTexture.getTexture());
		m_textureArray.finishInit();
	}

	a_database.unloadAsset(scene);
}

void GLScene::render(GLConstantBuffer& a_modelMatrixUBO)
{
	if (m_visible)
	{
		m_materialBuffer.bind();
		if (m_textureArray.getNumTexturesAdded())
			m_textureArray.bind(GLConfig::TEXTURE_ATLAS_ARRAY_BINDING_POINT);
		else
			m_textureArray.unbind(GLConfig::TEXTURE_ATLAS_ARRAY_BINDING_POINT);
		renderNode(a_modelMatrixUBO, m_baseTransform, 0);
	}
}

void GLScene::renderNode(GLConstantBuffer& a_modelMatrixUBO, const glm::mat4& a_parentTransform, uint a_node)
{
	const DBNode& node = m_nodes[a_node];
	glm::mat4 transform = a_parentTransform * node.getTransform();

	GLRenderer::ModelMatrixData* modelMatrix = (GLRenderer::ModelMatrixData*) a_modelMatrixUBO.mapBuffer();
	modelMatrix->u_modelMatrix = transform;
	a_modelMatrixUBO.unmapBuffer();

	for (uint i : node.getMeshIndices())
		m_meshes[i].render();

	for (uint i : node.getChildIndices())
		renderNode(a_modelMatrixUBO, transform, i);
}

void GLScene::setScale(float a_scale)
{
	m_scale = a_scale;
	updateBaseTransform();
}

void GLScene::setTranslation(const glm::vec3& a_translation)
{
	m_translation = a_translation;
	updateBaseTransform();
}

void GLScene::setRotation(const glm::vec3& a_axis, float a_degrees)
{
	m_axisRotation = glm::vec4(a_axis, a_degrees);
	updateBaseTransform();
}

void GLScene::updateBaseTransform()
{
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(m_scale));
	glm::mat4 translation = glm::translate(glm::mat4(1), m_translation);
	glm::mat4 rotation = glm::rotate(glm::mat4(1), m_axisRotation.w, glm::vec3(m_axisRotation.x, m_axisRotation.y, m_axisRotation.z));
	m_baseTransform = translation * rotation * scale;
}
