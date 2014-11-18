#include "Graphics/Scene/Scene.h"

#include "Graphics/PerspectiveCamera.h"
#include "Graphics/Scene/GL/GLScene.h"

Scene::Scene()
{
	m_glScene = new GLScene();
}

Model Scene::createModel(const glm::mat4& a_transform, const char* a_filePath)
{
	return Model();
}

Light Scene::createLight(const glm::vec3& a_position, const glm::vec3& a_color, float a_range)
{
	return Light();
}

