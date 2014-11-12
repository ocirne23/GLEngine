#pragma once

#include "Core.h"
#include "rde/vector.h"
#include "Model.h"
#include "Light.h"
#include "Graphics\GL\GLMesh.h"

#include "Utils\MatForward.h"
#include "Utils\VecForward.h"

class Camera;

class Scene;

template <typename T>
class Handle
{
public:
	T& get();
private:
	Scene* m_scene;
	uint m_itemIndex;
};

class Scene
{
public:
	Scene();
	~Scene();
	Scene(const Scene& copy) = delete;

	void render(const Camera& camera);
	Handle<Model> createModel(const glm::mat4& transform, const char* filePath);
	Handle<Light> createLight(const glm::vec3& position, const glm::vec3& color, float range);

	void remove(Handle<Model>& handle);
	void remove(Handle<Light>& handle);

private:

	rde::vector<Model> m_models;
	rde::vector<Light> m_lights;
	rde::vector<GLMesh> m_meshes;
};