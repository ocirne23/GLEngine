#pragma once

#include "Database/Assets/DBNode.h"
#include "Graphics/GL/Scene/GLMesh.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"

class AssetDatabase;

class GLScene
{
public:

	GLScene() {}
	void initialize(const eastl::string& assetName, AssetDatabase& database);

	void render(GLConstantBuffer& modelMatrixUBO);

public:

	eastl::vector<DBNode> m_nodes;
	eastl::vector<GLMesh> m_meshes;
	GLConstantBuffer m_materialBuffer;
	GLTextureArray m_textureArray;
};