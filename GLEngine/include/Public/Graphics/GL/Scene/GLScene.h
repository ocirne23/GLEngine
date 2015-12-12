#pragma once

#include "Database/Assets/DBNode.h"
#include "Graphics/GL/Scene/GLMesh.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"
#include <glm/glm.hpp>

class AssetDatabase;

class GLScene
{
public:

	GLScene() {}
	void initialize(const eastl::string& assetName, AssetDatabase& database);

	void render(GLConstantBuffer& modelMatrixUBO);

	void setScale(float scale);
	void setTranslation(const glm::vec3& translation);
	void setRotation(const glm::vec3& axis, float degrees);
	void setVisibility(bool a_visible) { m_visible = a_visible; }

	float getScale() const                   { return m_scale; }
	const glm::vec3& getTranslation() const  { return m_translation; }
	const glm::vec4& getAxisRotation() const { return m_axisRotation; }
	bool isVisible() const                   { return m_visible; }

private:

	void updateBaseTransform();
	void renderNode(GLConstantBuffer& modelMatrixUBO, const glm::mat4& parentTransform, uint node);

public:

	bool m_visible = true;
	float m_scale = 1.0f;
	glm::vec3 m_translation = glm::vec3(0);
	glm::vec4 m_axisRotation = glm::vec4(0, 1, 0, 0);

	glm::mat4 m_baseTransform;
	eastl::vector<DBNode> m_nodes;
	eastl::vector<GLMesh> m_meshes;
	GLConstantBuffer m_materialBuffer;
	GLTextureArray m_textureArray;
};