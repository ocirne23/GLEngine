#pragma once 

#include "Core.h"
#include "Light.h"
#include "Model.h"

#include "Utils\MatForward.h"
#include "Utils\VecForward.h"

#include "rde\vector.h"

class GLScene;
class PerspectiveCamera;

class Scene
{
public:
	template <typename T>
	class Handle
	{
	public:
		T& get();
	private:
		Scene* m_scene;
		uint m_itemIndex;
	};
public:

	Scene();
	~Scene();
	Scene(const Scene& copy) = delete;

	void render(const PerspectiveCamera& camera);
	Handle<Model> createModel(const glm::mat4& transform, const char* filePath);
	Handle<Light> creatóeLight(const glm::vec3& position, const glm::vec3& color, float range);

	void removeModel(Handle<Model>& modelHandle);
	void removeLight(Handle<Light>& lightHandle);

private:

	GLScene* m_glScene;

	rde::vector<Model> m_models;
	rde::vector<Light> m_lights;
};