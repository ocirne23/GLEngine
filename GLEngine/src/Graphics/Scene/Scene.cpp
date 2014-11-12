#include "Graphics\Scene\Scene.h"

Scene::Scene()
{
	Handle<Model> h = createModel(glm::mat4(), "da");
	h.get();
	remove(h);
}

Handle<Model> Scene::createModel(const glm::mat4& transform, const char* filePath)
{
	return Handle<Model>();
}

Handle<Light> Scene::createLight(const glm::vec3& position, const glm::vec3& color, float range)
{
	return Handle<Light>();
}