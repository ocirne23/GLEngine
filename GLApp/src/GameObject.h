#pragma once

#include "Graphics/GL/Scene/GLRenderObject.h"
#include "Graphics/GL/Scene/GLScene.h"

class GameObject : public GLRenderObject
{
public:

	GameObject() {}
	virtual ~GameObject() {}

	void initialize(GLScene* a_scene) { m_scene = a_scene; }

	virtual void render(const GLRenderer& renderer, GLConstantBuffer& modelMatrixUBO, bool depthOnly) override;

	void setPosition(const glm::vec3& a_position) { m_position = a_position; m_dirty = true; }
	void setScale(float a_scale) { m_scale = a_scale; m_dirty = true; }
	void setRotation(const glm::vec3& a_axis, float a_degrees) { m_axisRotation = glm::vec4(a_axis, a_degrees); m_dirty = true; }
	
private:
	
	void updateTransform();

private:

	glm::vec3 m_position;
	float m_scale            = 1.0f;
	glm::vec4 m_axisRotation = glm::vec4(0, 1, 0, 0);
	bool m_dirty             = true;
	glm::mat4 m_transform;
	GLScene* m_scene         = NULL;
};