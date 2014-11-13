#include "Graphics\Scene\Scene.h"

#include "Graphics\PerspectiveCamera.h"

Scene::Scene()
{
	Handle<Model> h = createModel(glm::mat4(), "da");
	h.get();
	remove(h);
}

void Scene::render(const PerspectiveCamera& camera)
{
	m_glScene->render(camera, m_models);
}

Scene::Handle<Model> Scene::createModel(const glm::mat4& a_transform, const char* a_filePath)
{
	return Handle<Model>();
}

Scene::Handle<Light> Scene::createLight(const glm::vec3& a_position, const glm::vec3& a_color, float a_range)
{
	return Handle<Light>();
}

template<>
Light& Scene::Handle<Light>::get() 
{
	return m_scene->m_lights[m_itemIndex];
}

template<>
Model& Scene::Handle<Model>::get()
{
	return m_scene->m_models[m_itemIndex];
}