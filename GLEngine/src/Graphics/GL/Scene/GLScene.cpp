#include "Graphics/GL/Scene/GLScene.h"

#include "Graphics/GL/Scene/GLMaterial.h"
#include "Graphics/GL/Scene/GLRenderer.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"

void GLScene::initialize(const eastl::string& a_assetName, AssetDatabase& a_database)
{
	DBScene* scene = (DBScene*) a_database.loadAsset(a_assetName, EAssetType::SCENE);

	m_nodes = scene->getNodes();

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

void GLScene::render(GLConstantBuffer& modelMatrixUBO)
{
	GLRenderer::ModelMatrixUBO* modelMatrix = (GLRenderer::ModelMatrixUBO*) modelMatrixUBO.mapBuffer();
	modelMatrix->u_modelMatrix = glm::mat4(1);
	modelMatrixUBO.unmapBuffer();

	for (GLMesh& mesh : m_meshes)
		mesh.render();
}
