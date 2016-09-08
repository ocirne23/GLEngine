#pragma once

#include "Database/Assets/DBNode.h"
#include "Database/Assets/DBMaterial.h"
#include "Graphics/GL/Scene/GLMesh.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"
#include <glm/glm.hpp>

class AssetDatabase;
class DBScene;
class GLRenderer;

class GLScene
{
public:

	GLScene() {}
	virtual ~GLScene() {}

	void initialize(const eastl::string& assetName, AssetDatabase& database);

	void initialize(const DBScene& dbScene);
	void render(GLRenderer& a_renderer, const glm::mat4& transform, bool depthOnly = false);

protected:

	void updateMaterialBuffer(const DBMaterial& material, uint idx);
	void updateMaterialBuffer();

private:

	void renderNode(const DBNode& node, GLRenderer& a_renderer, const glm::mat4& parentTransform);

protected:

	eastl::vector<DBNode> m_nodes;
	eastl::vector<DBMaterial> m_materials;
	eastl::vector<GLMesh> m_meshes;
	GLConstantBuffer m_materialBuffer;
	GLTextureArray m_textureArrays[DBMaterial::ETexTypes_COUNT];
};